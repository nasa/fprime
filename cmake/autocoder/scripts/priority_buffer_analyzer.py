#!/usr/bin/env python3
"""
Priority Buffer Size Analyzer

Analyzes F' components to compute maximum buffer sizes per priority level.
Uses fprime-fpp-python to analyze the topology's FPP model in-process.

Author: B. Duckett
Assisted by: Claude 4.5 Sonnet

Copyright 2026, by the California Institute of Technology.
ALL RIGHTS RESERVED. United States Government Sponsorship acknowledged.
"""

import sys
import argparse
import logging
import traceback
from pathlib import Path
from typing import Dict, List, Optional, Set, Tuple
from datetime import datetime
from dataclasses import dataclass, field

import fpp

# Initialize logger
logger = logging.getLogger(__name__)

# Files written into each module's build directory by the F Prime CMake system
FPP_SOURCE_LIST_FILE = "fpp-source-list"
FPP_IMPORT_LIST_FILE = "fpp-import-list"
SOURCE_LOCATIONS_FILE = "fprime-source-locations.fprime-util"
BINARY_LOCATIONS_FILE = "fprime-binary-locations.fprime-util"

# Default buffer sizes
INTERNAL_PORT_BUFFER_SIZE = "sizeof(FwIndexType)"


@dataclass
class PortBufferInfo:
    """Information about a port's buffer requirements at a specific priority"""

    port_name: str
    port_type: str
    buffer_expr: str
    is_internal: bool = False


@dataclass
class ComponentPriorityInfo:
    """Priority buffer information for a single component"""

    component_name: str
    component_namespace: str
    component_path: str
    priority_buffers: Dict[int, List[PortBufferInfo]] = field(default_factory=dict)

    @property
    def cpp_namespace(self) -> str:
        """Convert component path to C++ namespace identifier"""
        return self.component_path.replace("/", "_").replace("-", "_")


def to_cpp_name(qualified_name: str) -> str:
    """Convert a dotted FPP qualified name to a C++ qualified name"""
    return qualified_name.replace(".", "::")


class PriorityBufferAnalyzer:
    """Analyzes F' components using fprime-fpp-python to compute priority buffer sizes"""

    def __init__(
        self,
        build_dir: Path,
        topology_path: Path,
        output_file: Path,
        verbose: bool = False,
    ):
        self.build_dir = build_dir
        self.topology_path = topology_path
        self.output_file = output_file
        self.verbose = verbose
        self.component_headers: Set[str] = set()
        self.model: Optional[fpp.Model] = None
        self.location_pairs: List[Tuple[Path, Path]] = []

    def load_location_pairs(self) -> List[Tuple[Path, Path]]:
        """Load (source, binary) location pairs, longest source path first

        Raises:
            FileNotFoundError: If the location files are missing from the build directory
            ValueError: If the source and binary locations do not pair up
        """
        sources = (self.build_dir / SOURCE_LOCATIONS_FILE).read_text().split()
        binaries = (self.build_dir / BINARY_LOCATIONS_FILE).read_text().split()
        if len(sources) != len(binaries):
            raise ValueError(
                f"Mismatched source and binary locations in {self.build_dir}"
            )
        pairs = [
            (Path(s).resolve(), Path(b).resolve()) for s, b in zip(sources, binaries)
        ]
        return sorted(pairs, key=lambda pair: len(pair[0].parts), reverse=True)

    def run(self) -> int:
        """Main analysis entry point

        Returns:
            Number of components that failed to analyze
        """
        logger.info(f"Loading topology from: {self.topology_path}")

        self.location_pairs = self.load_location_pairs()
        self.model = self.load_topology_model()
        topology = self.extract_topology(self.model)
        components = self.get_components(topology)

        logger.info(f"Found {len(components)} components in topology")

        component_results = []
        fail_count = 0
        for component in components:
            try:
                result = self.analyze_component(component)
                if result:
                    component_results.append(result)
                    logger.info(
                        f"  ✓ {result.component_path}: priorities {sorted(result.priority_buffers.keys())}"
                    )
            except (FileNotFoundError, ValueError, KeyError, AttributeError) as e:
                logger.info(
                    f"  ✗ Failed to analyze {component.symbol.qualified_name}: {e}"
                )
                if self.verbose:
                    traceback.print_exc()
                fail_count += 1

        if component_results:
            logger.info(f"Generating header for {len(component_results)} components...")
            self.generate_header(component_results)
            logger.info(f"Generated: {self.output_file}")
        else:
            logger.info("No components with multiple priorities found")
            self.generate_empty_header()
        return fail_count

    def load_topology_model(self) -> fpp.Model:
        """Analyze the topology module's FPP sources along with their imports

        Raises:
            FileNotFoundError: If the topology build directory lacks the FPP source lists
            ValueError: If the FPP model has errors
        """
        sources = self._read_list_file(self.topology_path / FPP_SOURCE_LIST_FILE)
        imports = self._read_list_file(self.topology_path / FPP_IMPORT_LIST_FILE)
        if not sources:
            raise ValueError(f"No FPP sources listed in {self.topology_path}")

        model = fpp.analyze(sources, imports=imports)
        for diagnostic in model.diagnostics:
            print(diagnostic.render(color=sys.stderr.isatty()), file=sys.stderr)
        if model.has_errors:
            raise ValueError(f"FPP model has {model.error_count} error(s)")
        return model

    @staticmethod
    def _read_list_file(list_file: Path) -> List[str]:
        """Read a CMake list (semicolon-separated) written by the build system"""
        return [entry for entry in list_file.read_text().strip().split(";") if entry]

    def extract_topology(self, model: fpp.Model) -> fpp.Topology:
        """Select the deployment topology defined by the topology module's own sources

        Raises:
            ValueError: If no topology found in analysis
        """
        topologies = [
            topology
            for topology in model.analysis.topology_map.values()
            if topology.node.in_source
        ]
        if not topologies:
            raise ValueError("No topology found in analysis")

        deployments = [t for t in topologies if t.node.is_deployment]
        topology = (deployments or topologies)[0]
        logger.debug(f"Loaded topology: {topology.qualified_name}")
        return topology

    def get_components(self, topology: fpp.Topology) -> List[fpp.Component]:
        """Collect the unique components instantiated in the topology"""
        components: Dict[fpp.Symbol, fpp.Component] = {}

        for comp_instance in topology.component_instance_map:
            logger.debug(f"  Component instance: {comp_instance.qualified_name}")
            component = comp_instance.component
            if component is not None:
                components.setdefault(component.symbol, component)

        return list(components.values())

    def _should_skip_component(
        self, priority_buffers: Dict[int, List[PortBufferInfo]]
    ) -> bool:
        """Check if component should be skipped based on priority buffers"""
        if not priority_buffers:
            logger.debug("  No async ports or commands found")
            return True

        unique_priorities = set(priority_buffers.keys())
        if len(unique_priorities) == 1 and 0 in unique_priorities:
            logger.debug("  Only priority 0, skipping")
            return True

        return False

    def _create_component_info(
        self,
        component: fpp.Component,
        component_dir: Path,
        priority_buffers: Dict[int, List[PortBufferInfo]],
    ) -> ComponentPriorityInfo:
        """Create ComponentPriorityInfo from component data"""
        symbol = component.symbol
        relative_path = component_dir.relative_to(self.build_dir).as_posix()
        self.component_headers.add(
            f"{relative_path}/{symbol.unqualified_name}ComponentAc.hpp"
        )

        return ComponentPriorityInfo(
            component_name=symbol.unqualified_name,
            component_namespace=symbol.parent.qualified_name if symbol.parent else "",
            component_path=relative_path,
            priority_buffers=priority_buffers,
        )

    def analyze_component(
        self, component: fpp.Component
    ) -> Optional[ComponentPriorityInfo]:
        """Analyze a component for priority buffer requirements"""
        comp_qualified_name = component.symbol.qualified_name
        logger.debug(f"Analyzing: {comp_qualified_name}")

        component_dir = self.find_component_build_dir(component)
        if not component_dir:
            logger.debug(f"  Could not find build directory for {comp_qualified_name}")
            return None

        priority_buffers = self.extract_priority_buffers(component)
        if self._should_skip_component(priority_buffers):
            return None

        return self._create_component_info(component, component_dir, priority_buffers)

    def find_component_build_dir(self, component: fpp.Component) -> Optional[Path]:
        """Find component's build directory by mapping its source directory through the location pairs"""
        source_dir = Path(component.node.location.uri).resolve().parent
        logger.debug(f"  Source directory: {source_dir}")

        for source_location, binary_location in self.location_pairs:
            try:
                relative_dir = source_dir.relative_to(source_location)
            except ValueError:
                continue
            build_path = binary_location / relative_dir
            if build_path.is_dir():
                logger.debug(f"  Found at: {build_path}")
                return build_path
            return None
        return None

    def extract_priority_buffers(
        self, component: fpp.Component
    ) -> Dict[int, List[PortBufferInfo]]:
        """Extract priority -> buffer info mapping for component"""
        priority_map: Dict[int, List[PortBufferInfo]] = {}

        self.extract_async_ports(component, priority_map)
        self.extract_async_commands(component, priority_map)

        return priority_map

    def extract_async_ports(
        self,
        component: fpp.Component,
        priority_map: Dict[int, List[PortBufferInfo]],
    ) -> None:
        """Extract async input ports and internal ports"""
        for port_name, port_instance in component.port_map.items():
            if isinstance(port_instance, fpp.PortInstance.General):
                if isinstance(port_instance.kind, fpp.GeneralKind.AsyncInput):
                    priority = self.get_port_priority(port_instance)
                    buffer_expr = self.get_port_buffer_size_expr(port_instance)

                    if buffer_expr:
                        if priority not in priority_map:
                            priority_map[priority] = []

                        port_type = self.get_port_type_name(port_instance)
                        priority_map[priority].append(
                            PortBufferInfo(
                                port_name=str(port_name),
                                port_type=port_type,
                                buffer_expr=buffer_expr,
                                is_internal=False,
                            )
                        )
                        logger.debug(
                            f"  Async port '{port_name}' at priority {priority}: {buffer_expr}"
                        )

            elif isinstance(port_instance, fpp.PortInstance.Internal):
                priority = self.get_port_priority(port_instance)
                buffer_expr = self.calculate_internal_port_buffer(port_instance)

                if buffer_expr:
                    if priority not in priority_map:
                        priority_map[priority] = []

                    priority_map[priority].append(
                        PortBufferInfo(
                            port_name=str(port_name),
                            port_type="Internal",
                            buffer_expr=buffer_expr,
                            is_internal=True,
                        )
                    )
                    logger.debug(
                        f"  Internal port '{port_name}' at priority {priority}: {buffer_expr}"
                    )

    def extract_async_commands(
        self,
        component: fpp.Component,
        priority_map: Dict[int, List[PortBufferInfo]],
    ) -> None:
        """Extract async commands and add Fw::CmdPortBuffer"""
        async_command_priorities = set()

        for opcode, command in component.command_map.items():
            if isinstance(command, fpp.Command.NonParam):
                if isinstance(command.kind, fpp.NonParamKind.Async):
                    priority = self.get_command_priority(command)
                    async_command_priorities.add(priority)
                    logger.debug(
                        f"  Async command '{command.name}' at priority {priority}"
                    )

        port_type = "Fw::Cmd"
        for priority in async_command_priorities:
            if priority not in priority_map:
                priority_map[priority] = []

            has_cmd = any(
                p.buffer_expr == self.get_qual_port_buffer_size_expr(port_type)
                for p in priority_map[priority]
            )

            if not has_cmd:
                priority_map[priority].append(
                    PortBufferInfo(
                        port_name="CmdRecv",
                        port_type="Cmd",
                        buffer_expr=self.get_qual_port_buffer_size_expr(port_type),
                        is_internal=False,
                    )
                )
                logger.debug(f"  Added {port_type} to priority {priority}")

    def get_port_priority(self, port_instance: fpp.PortInstance) -> int:
        """Extract the resolved priority from a port instance, defaulting to 0"""
        priority_val = None

        if isinstance(port_instance, fpp.PortInstance.General):
            if isinstance(port_instance.kind, fpp.GeneralKind.AsyncInput):
                priority_val = port_instance.kind.priority
        elif isinstance(
            port_instance, (fpp.PortInstance.Special, fpp.PortInstance.Internal)
        ):
            priority_val = port_instance.priority

        return 0 if priority_val is None else priority_val

    def get_command_priority(self, command: fpp.Command.NonParam) -> int:
        """Extract the resolved priority from an async command, defaulting to 0"""
        if isinstance(command.kind, fpp.NonParamKind.Async):
            if command.kind.priority is not None:
                return command.kind.priority
        return 0

    def _get_port_symbol(
        self, port_instance: fpp.PortInstance.General
    ) -> Optional[fpp.Symbol]:
        """Get the symbol of the port definition used by a typed port instance"""
        port_type = port_instance.ty
        if isinstance(port_type, fpp.PortInstanceType.DefPort):
            return self.model.analysis.symbol_map[port_type.definition.node_id]
        return None

    def get_port_type_name(self, port_instance: fpp.PortInstance.General) -> str:
        """Get qualified port type name"""
        symbol = self._get_port_symbol(port_instance)
        return symbol.qualified_name if symbol else "Unknown"

    def get_port_buffer_size_expr(self, port_instance: fpp.PortInstance.General) -> str:
        """Get C++ buffer expression for port"""
        symbol = self._get_port_symbol(port_instance)
        if symbol:
            return self.get_qual_port_buffer_size_expr(
                to_cpp_name(symbol.qualified_name)
            )
        return ""

    def get_qual_port_buffer_size_expr(self, qualified_str: str) -> str:
        """Get C++ buffer expression for port"""
        return f"{qualified_str}PortBuffer::CAPACITY"

    def _is_primitive_type(self, resolved_type: fpp.Type) -> bool:
        """Check if type is a primitive type (int, float, or bool)"""
        return isinstance(
            resolved_type,
            (fpp.Type.PrimitiveInt, fpp.Type.Float, fpp.Type.Boolean),
        )

    def resolved_type_to_cpp_size_expr(self, resolved_type: fpp.Type) -> str:
        """Convert a resolved semantic Type to a C++ serialized-size expression.

        Raises:
            ValueError: If type cannot be handled
        """
        if isinstance(resolved_type, (fpp.Type.PrimitiveInt, fpp.Type.Float)):
            return f"sizeof({resolved_type.value.name})"
        elif isinstance(resolved_type, fpp.Type.Boolean):
            return "sizeof(FwEnumStoreType)"
        elif isinstance(resolved_type, fpp.Type.String):
            if resolved_type.value:
                return f"Fw::StringBase::STATIC_SERIALIZED_SIZE({resolved_type.value})"
            return "Fw::StringBase::SERIALIZED_SIZE"
        elif isinstance(resolved_type, fpp.Type.Alias):
            underlying = resolved_type.underlying_type
            if self._is_primitive_type(underlying):
                cpp_name = to_cpp_name(resolved_type.def_symbol.qualified_name)
                return f"sizeof({cpp_name})"
            else:
                return self.resolved_type_to_cpp_size_expr(underlying)
        elif isinstance(
            resolved_type,
            (fpp.Type.Abs, fpp.Type.Struct, fpp.Type.Array, fpp.Type.Enum),
        ):
            cpp_name = to_cpp_name(resolved_type.def_symbol.qualified_name)
            return f"{cpp_name}::SERIALIZED_SIZE"
        else:
            raise ValueError(f"Unhandled type: {resolved_type!r}")

    def calculate_internal_port_buffer(
        self, port_instance: fpp.PortInstance.Internal
    ) -> str:
        """Calculate buffer size expression for internal port by summing parameter sizes

        Raises:
            ValueError: If a parameter type was not resolved
        """
        params = port_instance.node.params
        if not params:
            return "0"

        size_expressions = []
        for formal_param in params:
            logger.debug(f"    param: {formal_param.name}")
            resolved_type = formal_param.type_name.resolved_type
            if resolved_type is None:
                raise ValueError(
                    f"Could not resolve type of parameter '{formal_param.name}' at {formal_param.location.display}"
                )

            type_size_expr = self.resolved_type_to_cpp_size_expr(resolved_type)
            logger.debug(
                f"    type_size_expr: {type_size_expr!r}, resolved_type: {resolved_type!r}"
            )
            size_expressions.append(type_size_expr)

        return " + ".join(size_expressions)

    def _generate_header_preamble(self, lines: List[str]) -> None:
        """Generate header file preamble with includes"""
        lines.append("#ifndef PRIORITY_BUFFER_SIZES_AC_HPP")
        lines.append("#define PRIORITY_BUFFER_SIZES_AC_HPP")
        lines.append("")
        lines.append(f"// Auto-generated by {__file__}")
        lines.append("// DO NOT EDIT")
        lines.append(f"// Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        lines.append("")
        lines.append("#include <Fw/Types/BasicTypes.hpp>")
        lines.append("")

        if self.component_headers:
            lines.append("// Component headers")
            for header in sorted(self.component_headers):
                lines.append(f"#include <{header}>")
            lines.append("")

    def _get_unique_buffer_exprs(
        self, buffers: List[PortBufferInfo]
    ) -> Tuple[List[str], List[str]]:
        """Extract unique buffer expressions and port types, preserving order"""
        unique_map = {b.buffer_expr: b.port_type for b in buffers}
        return list(unique_map.keys()), list(unique_map.values())

    def _generate_priority_constant(
        self,
        lines: List[str],
        priority: int,
        unique_exprs: List[str],
        port_list: str,
        comp_path: str,
    ) -> None:
        """Generate constant definition for a single priority level

        Raises:
            ValueError: If no buffer expressions provided
        """
        lines.append(f"    // Priority {priority}: {port_list}")
        lines.append(f"    static constexpr FwSizeType PRIORITY_{priority} =")

        if len(unique_exprs) == 0:
            raise ValueError(
                f"No buffer expressions found for {comp_path} priority {priority}"
            )
        elif len(unique_exprs) == 1:
            lines.append(f"        {unique_exprs[0]} + DATA_OFFSET;")
        else:
            result = unique_exprs[-1]
            for expr in reversed(unique_exprs[:-1]):
                result = f"FW_MAX({expr}, {result})"
            lines.append(f"        {result} + DATA_OFFSET;")
        lines.append("")

    def generate_header(self, components: List[ComponentPriorityInfo]) -> None:
        """Generate C++ header file with priority buffer constants"""
        lines = []
        self._generate_header_preamble(lines)

        lines.append("namespace PriorityBufferConfig {")
        lines.append("")
        lines.append(
            "constexpr FwSizeType DATA_OFFSET = sizeof(FwEnumStoreType) + sizeof(FwIndexType);"
        )
        lines.append("")

        for comp_info in sorted(components, key=lambda c: c.component_path):
            lines.append(f"// Component: {comp_info.component_path}")
            lines.append(f"namespace {comp_info.cpp_namespace} {{")
            lines.append("")

            for priority in sorted(comp_info.priority_buffers.keys()):
                buffers = comp_info.priority_buffers[priority]
                unique_exprs, unique_port_types = self._get_unique_buffer_exprs(buffers)
                port_list = ", ".join(unique_port_types)
                self._generate_priority_constant(
                    lines, priority, unique_exprs, port_list, comp_info.component_path
                )

            lines.append(f"}} // namespace {comp_info.cpp_namespace}")
            lines.append("")

        lines.append("} // namespace PriorityBufferConfig")
        lines.append("")
        lines.append("#endif // PRIORITY_BUFFER_SIZES_AC_HPP")

        self.output_file.parent.mkdir(parents=True, exist_ok=True)
        with open(self.output_file, "w") as f:
            f.write("\n".join(lines))

    def generate_empty_header(self):
        """Generate empty header when no components found"""
        lines = [
            "#ifndef PRIORITY_BUFFER_SIZES_AC_HPP",
            "#define PRIORITY_BUFFER_SIZES_AC_HPP",
            "",
            "// Auto-generated by priority_buffer_analyzer.py",
            "// No components with multiple priorities found",
            "",
            "namespace PriorityBufferConfig {",
            "} // namespace PriorityBufferConfig",
            "",
            "#endif // PRIORITY_BUFFER_SIZES_AC_HPP",
        ]

        self.output_file.parent.mkdir(parents=True, exist_ok=True)
        with open(self.output_file, "w") as f:
            f.write("\n".join(lines))


def main():
    parser = argparse.ArgumentParser(
        description="Analyze F' components for priority buffer sizes using fprime-fpp-python",
    )
    parser.add_argument(
        "--build-dir",
        type=Path,
        required=True,
        help="F-Prime build directory (e.g., build-fprime-automatic-native)",
    )
    parser.add_argument(
        "--topology-path",
        type=Path,
        required=True,
        help="Build directory of the topology module (e.g., build-dir/Deployment/Top)",
    )
    parser.add_argument(
        "--output", type=Path, required=True, help="Output header file path"
    )
    parser.add_argument(
        "--verbose",
        "-v",
        action="store_true",
        help="Print detailed analysis information",
    )

    args = parser.parse_args()

    # Configure logging
    log_level = logging.DEBUG if args.verbose else logging.INFO
    logging.basicConfig(level=log_level, format="%(levelname)s: %(message)s")

    if not args.build_dir.exists():
        logger.error(f"Build directory not found: {args.build_dir}")
        return 1

    if not args.topology_path.exists():
        logger.error(f"Topology path not found: {args.topology_path}")
        return 1

    analyzer = PriorityBufferAnalyzer(
        build_dir=args.build_dir.resolve(),
        topology_path=args.topology_path.resolve(),
        output_file=args.output,
        verbose=args.verbose,
    )

    try:
        fail_count = analyzer.run()
        return fail_count
    except fpp.DiagnosticError as e:
        print(e.diagnostic.render(color=sys.stderr.isatty()), file=sys.stderr)
        return 1
    except Exception as e:
        logger.error(f"Error: {e}")
        if args.verbose:
            traceback.print_exc()
        return 1


if __name__ == "__main__":
    sys.exit(main())
