#!/usr/bin/env python3
"""
Priority Buffer Size Analyzer - fprime_python_model Implementation

Analyzes F' components to compute maximum buffer sizes per priority level.
Uses fprime_python_model for parsing FPP JSON artifacts (no direct JSON parsing).

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
from typing import Any, Dict, List, Optional, Set, Tuple
from datetime import datetime
from dataclasses import dataclass, field

# Note: fpm_ prefixes on imports are intentional to make it clear which
# classes are from fprime_python_model vs. local definitions
from fprime_python_model.model import FprimePythonModel as fpm_FprimePythonModel
from fprime_python_model.semantics.analysis import Analysis as fpm_Analysis
from fprime_python_model.semantics.topology import Topology as fpm_Topology
from fprime_python_model.semantics.component import Component as fpm_Component
from fprime_python_model.semantics.component_instance import (
    ComponentInstance as fpm_ComponentInstance,
)
from fprime_python_model.semantics import port_instance as fpm_port_instance
from fprime_python_model.semantics import command as fpm_command
from fprime_python_model.semantics.interface_instance import (
    InterfaceComponentInstance as fpm_InterfaceComponentInstance,
)
from fprime_python_model.semantics import types_values as fpm_types_values
from fprime_python_model import fpp_ast as fpm_fpp_ast
from fprime_python_model.semantics.symbol import Symbol as fpm_Symbol

# Initialize logger
logger = logging.getLogger(__name__)

# Build path patterns
FPRIME_SOURCE_PATTERN = "/lib/fprime/"
FPRIME_BUILD_PREFIX = "F-Prime"

# Default buffer sizes
INTERNAL_PORT_BUFFER_SIZE = "sizeof(FwIndexType)"

# JSON file names
JSON_AST_FILE = "fpp-ast.json"
JSON_LOCATIONS_FILE = "fpp-loc-map.json"
JSON_ANALYSIS_FILE = "fpp-analysis.json"


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


class PriorityBufferAnalyzer:
    """Analyzes F' components using fprime_python_model to compute priority buffer sizes"""

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
        self.topology_model: Optional[fpm_FprimePythonModel] = None

    def _validate_component_json_files(self, directory: Path) -> bool:
        """Check if all required FPP JSON files exist in directory"""
        required = [JSON_AST_FILE, JSON_LOCATIONS_FILE, JSON_ANALYSIS_FILE]
        return all((directory / f).exists() for f in required)

    def _load_fprime_model(self, json_dir: Path) -> Optional[fpm_FprimePythonModel]:
        """Load fpm_FprimePythonModel from directory with standard JSON files"""
        if not self._validate_component_json_files(json_dir):
            return None
        try:
            return fpm_FprimePythonModel(
                str(json_dir / JSON_AST_FILE),
                str(json_dir / JSON_LOCATIONS_FILE),
                str(json_dir / JSON_ANALYSIS_FILE),
            )
        except (FileNotFoundError, ValueError) as e:
            logger.error(f"Failed to load model from {json_dir}: {e}")
            return None

    def run(self) -> int:
        """Main analysis entry point

        Returns:
            Number of components that failed to analyze
        """
        logger.info(f"Loading topology from: {self.topology_path}")

        self.topology_model = self.load_topology_model()
        topology = self.extract_topology(self.topology_model)
        component_instances = self.get_component_instances(topology)

        logger.info(f"Found {len(component_instances)} component instances in topology")

        component_results = []
        fail_count = 0
        for comp_instance in component_instances:
            try:
                result = self.analyze_component_instance(comp_instance)
                if result:
                    component_results.append(result)
                    logger.info(
                        f"  ✓ {result.component_path}: priorities {sorted(result.priority_buffers.keys())}"
                    )
            except (FileNotFoundError, ValueError, KeyError, AttributeError) as e:
                logger.info(
                    f"  ✗ Failed to analyze {comp_instance.get_qualified_name()}: {e}"
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

    def load_topology_model(self) -> fpm_FprimePythonModel:
        """Load topology using fpm_FprimePythonModel

        Raises:
            FileNotFoundError: If required JSON files are missing
        """
        topology_dir = self.topology_path

        if not self._validate_component_json_files(topology_dir):
            raise FileNotFoundError(f"Missing required JSON files in {topology_dir}")

        return fpm_FprimePythonModel(
            str(topology_dir / JSON_AST_FILE),
            str(topology_dir / JSON_LOCATIONS_FILE),
            str(topology_dir / JSON_ANALYSIS_FILE),
        )

    def extract_topology(self, model: fpm_FprimePythonModel) -> fpm_Topology:
        """Extract fpm_Topology from fpm_Analysis.topology_map

        Raises:
            ValueError: If no topology found in analysis
        """
        analysis = model.analysis

        if not analysis.topology_map:
            raise ValueError("No topology found in analysis")

        topology = next(iter(analysis.topology_map.values()))
        logger.debug(f"Loaded topology: {topology.get_qualified_name()}")
        return topology

    def get_component_instances(
        self, topology: fpm_Topology
    ) -> List[fpm_ComponentInstance]:
        """Extract fpm_ComponentInstance objects from fpm_Topology.instance_map"""
        component_instances = []

        for interface_instance, location in topology.instance_map.items():
            if isinstance(interface_instance, fpm_InterfaceComponentInstance):
                comp_instance = interface_instance.ci
                component_instances.append(comp_instance)
                logger.debug(
                    f"  Component instance: {comp_instance.get_qualified_name()}"
                )

        return component_instances

    def _load_component_from_model(
        self, comp_model: fpm_FprimePythonModel, component_dir: Path
    ) -> fpm_Component:
        """Load single component from component model

        Raises:
            ValueError: If no components found or multiple components in single file
        """
        comp_analysis = comp_model.analysis
        if not comp_analysis.component_map:
            raise ValueError("No components in component_map")

        comp_ids = list(comp_analysis.component_map.keys())
        if len(comp_ids) != 1:
            raise ValueError(
                f"Multiple component definitions in single component file: {comp_ids} in {component_dir}"
            )

        return comp_analysis.component_map[comp_ids[0]]

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

    def _load_and_analyze_component(
        self, component_dir: Path
    ) -> Optional[Dict[int, List[PortBufferInfo]]]:
        """Load component model and extract priority buffers

        Returns:
            Priority buffer mapping or None if model loading fails

        Raises:
            ValueError: If component model is invalid
        """
        comp_model = self.load_component_model(component_dir)
        if not comp_model:
            return None

        loaded_component = self._load_component_from_model(comp_model, component_dir)
        return self.extract_priority_buffers(loaded_component, comp_model.analysis)

    def _create_component_info(
        self,
        component: fpm_Component,
        component_dir: Path,
        priority_buffers: Dict[int, List[PortBufferInfo]],
    ) -> ComponentPriorityInfo:
        """Create ComponentPriorityInfo from component data"""
        sym = fpm_Symbol.construct(component.a_node)
        component_name = self.topology_model.analysis.get_qualified_name_from_map(sym)
        relative_path = str(component_dir.relative_to(self.build_dir))
        self.component_headers.add(
            f"{relative_path}/{component_name.base}ComponentAc.hpp"
        )

        return ComponentPriorityInfo(
            component_name=component_name.base,
            component_namespace=component_name.qualifier,
            component_path=relative_path,
            priority_buffers=priority_buffers,
        )

    def analyze_component_instance(
        self, comp_instance: fpm_ComponentInstance
    ) -> Optional[ComponentPriorityInfo]:
        """Analyze a component instance for priority buffer requirements"""
        comp_qualified_name = comp_instance.get_qualified_name()
        logger.debug(f"Analyzing: {comp_qualified_name}")

        component = comp_instance.component
        component_dir = self.find_component_build_dir(comp_instance, component)
        if not component_dir:
            logger.debug(f"  Could not find build directory for {comp_qualified_name}")
            return None

        priority_buffers = self._load_and_analyze_component(component_dir)
        if priority_buffers is None:
            return None

        if self._should_skip_component(priority_buffers):
            return None

        return self._create_component_info(component, component_dir, priority_buffers)

    def _transform_source_to_build_path(self, source_path: str) -> Optional[Path]:
        """Transform source path to expected build path"""
        source_str = str(source_path)
        if FPRIME_SOURCE_PATTERN in source_str:
            # Extract path after lib/fprime/
            rel_path = source_str.split(FPRIME_SOURCE_PATTERN)[1]
            # Remove the .fpp filename
            rel_dir = str(Path(rel_path).parent)
            # Build path is F-Prime/<rel_dir>
            build_path = self.build_dir / FPRIME_BUILD_PREFIX / rel_dir

            if build_path.exists() and (build_path / JSON_ANALYSIS_FILE).exists():
                return build_path
        return None

    def find_component_build_dir(
        self, comp_instance: fpm_ComponentInstance, component: fpm_Component
    ) -> Optional[Path]:
        """Find component's build directory by deriving from source path"""
        comp_qualified_name = comp_instance.get_qualified_name()

        # Get the source path from component's AST node using topology_model
        if not self.topology_model:
            return None

        try:
            comp_def_node = component.a_node[1]
            source_path = self.topology_model.get_location(comp_def_node).path
            logger.debug(f"  Source path: {source_path}")

            # Try to transform source path to build path
            build_path = self._transform_source_to_build_path(source_path)
            if build_path:
                logger.debug(f"  Found at: {build_path}")
                return build_path

        except (AttributeError, KeyError, OSError) as e:
            logger.debug(f"  Error getting source path: {e}")

        return None

    def load_component_model(
        self, component_dir: Path
    ) -> Optional[fpm_FprimePythonModel]:
        """Load component model using fpm_FprimePythonModel"""
        model = self._load_fprime_model(component_dir)
        if not model:
            logger.debug(
                f"  Missing JSON files or failed to load model from {component_dir}"
            )
        return model

    def extract_priority_buffers(
        self, component: fpm_Component, analysis: fpm_Analysis
    ) -> Dict[int, List[PortBufferInfo]]:
        """Extract priority -> buffer info mapping for component"""
        priority_map: Dict[int, List[PortBufferInfo]] = {}

        self.extract_async_ports(component, analysis, priority_map)
        self.extract_async_commands(component, analysis, priority_map)

        return priority_map

    def extract_async_ports(
        self,
        component: fpm_Component,
        analysis: fpm_Analysis,
        priority_map: Dict[int, List[PortBufferInfo]],
    ) -> None:
        """Extract async input ports and internal ports"""
        for port_name, port_instance in component.port_map.items():
            if isinstance(port_instance, fpm_port_instance.GeneralPortInstance):
                if port_instance.kind == fpm_fpp_ast.fpp_ast.GeneralKind.ASYNC_INPUT:
                    priority = self.get_port_priority(port_instance, analysis)
                    buffer_expr = self.get_port_buffer_size_expr(
                        port_instance, analysis
                    )

                    if buffer_expr:
                        if priority not in priority_map:
                            priority_map[priority] = []

                        port_type = self.get_port_type_name(port_instance, analysis)
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

            elif isinstance(port_instance, fpm_port_instance.InternalPortInstance):
                priority = self.get_port_priority(port_instance, analysis)
                buffer_expr = self.calculate_internal_port_buffer(
                    port_instance, analysis
                )

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
        component: fpm_Component,
        analysis: fpm_Analysis,
        priority_map: Dict[int, List[PortBufferInfo]],
    ) -> None:
        """Extract async commands and add Fw::CmdPortBuffer"""
        async_command_priorities = set()

        for opcode, command in component.command_map.items():
            if isinstance(command, fpm_command.CommandNonParam):
                if isinstance(command.kind, fpm_command.NonParamKindAsync):
                    priority = self.get_command_priority(command, analysis)
                    async_command_priorities.add(priority)
                    logger.debug(
                        f"  Async command '{command.get_name()}' at priority {priority}"
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

    def get_port_priority(
        self, port_instance: fpm_port_instance.PortInstance, analysis: fpm_Analysis
    ) -> int:
        """Extract priority from port instance"""
        priority_val = None

        if isinstance(port_instance, fpm_port_instance.GeneralPortInstance):
            if port_instance.specifier.priority is not None:
                expr = port_instance.specifier.priority.data
                # Handle numeric literal
                if isinstance(expr, fpm_fpp_ast.fpp_ast.ExprLiteralInt):
                    priority_val = int(expr.value)
                # Handle named constant (e.g., "ActiveRateGroupOutputPorts")
                elif isinstance(expr, fpm_fpp_ast.fpp_ast.ExprIdent):
                    enum_name = expr.value
                    priority_val = analysis.value_map[
                        port_instance.specifier.priority._id
                    ].value
                else:
                    raise ValueError(f"Cannot interpret priority {expr!r}")
        elif isinstance(port_instance, fpm_port_instance.SpecialPortInstance):
            priority_val = port_instance.priority
        elif isinstance(port_instance, fpm_port_instance.InternalPortInstance):
            priority_val = port_instance.priority

        if priority_val is None:
            return 0
        elif isinstance(priority_val, int):
            return priority_val
        else:
            raise ValueError(f"Cannot interpret priority for port {port_instance!r}")

    def get_command_priority(
        self, command: fpm_command.CommandNonParam, analysis: fpm_Analysis
    ) -> int:
        """Extract priority from async command"""
        if isinstance(command.kind, fpm_command.NonParamKindAsync):
            priority_val = command.kind.priority

            if priority_val is None:
                return 0

            if isinstance(priority_val, int):
                return priority_val

        return 0

    def get_port_type_name(
        self,
        port_instance: fpm_port_instance.GeneralPortInstance,
        analysis: fpm_Analysis,
    ) -> str:
        """Get qualified port type name using fpm_Analysis.get_qualified_name_from_map"""
        port_type = port_instance.ty

        if isinstance(port_type, fpm_port_instance.DefPortPortInstanceType):
            symbol = port_type.symbol
            qualified_name = analysis.get_qualified_name_from_map(symbol)
            return str(qualified_name)

        return "Unknown"

    def get_port_buffer_size_expr(
        self,
        port_instance: fpm_port_instance.GeneralPortInstance,
        analysis: fpm_Analysis,
    ) -> str:
        """Get C++ buffer expression for port"""
        port_type = port_instance.ty

        if isinstance(port_type, fpm_port_instance.DefPortPortInstanceType):
            symbol = port_type.symbol
            qualified_name = analysis.get_qualified_name_from_map(symbol)

            qualified_str = str(qualified_name).replace(".", "::")

            return self.get_qual_port_buffer_size_expr(qualified_str)

        return ""

    def get_qual_port_buffer_size_expr(self, qualified_str: str) -> str:
        """Get C++ buffer expression for port"""
        return f"{qualified_str}PortBuffer::CAPACITY"

    def _is_primitive_type(self, resolved_type: fpm_types_values.Type) -> bool:
        """Check if type is a primitive type (int, float, or bool)"""
        return isinstance(
            resolved_type,
            (
                fpm_types_values.PrimitiveIntType,
                fpm_types_values.FloatType,
                fpm_types_values.BooleanType,
            ),
        )

    def resolved_type_to_cpp_size_expr(
        self, resolved_type: fpm_types_values.Type, analysis: fpm_Analysis
    ) -> str:
        """Convert a resolved semantic Type to a C++ serialized-size expression.

        Raises:
            ValueError: If type cannot be handled
        """
        if isinstance(resolved_type, fpm_types_values.PrimitiveIntType):
            return f"sizeof({resolved_type.kind.name})"
        elif isinstance(resolved_type, fpm_types_values.FloatType):
            return f"sizeof({resolved_type.kind.name})"
        elif isinstance(resolved_type, fpm_types_values.BooleanType):
            return "sizeof(FwEnumStoreType)"
        elif isinstance(resolved_type, fpm_types_values.StringType):
            if resolved_type.size:
                return f"Fw::StringBase::STATIC_SERIALIZED_SIZE({resolved_type.size})"
            return "Fw::StringBase::SERIALIZED_SIZE"
        elif isinstance(resolved_type, fpm_types_values.AliasType):
            underlying = resolved_type.get_underlying_type()
            if self._is_primitive_type(underlying):
                symbol = resolved_type.get_def_symbol()
                qn = analysis.get_qualified_name_from_map(symbol)
                cpp_name = "::".join(qn.to_ident_list())
                return f"sizeof({cpp_name})"
            else:
                return self.resolved_type_to_cpp_size_expr(underlying, analysis)
        elif isinstance(
            resolved_type,
            (
                fpm_types_values.AbsType,
                fpm_types_values.StructType,
                fpm_types_values.ArrayType,
                fpm_types_values.EnumType,
            ),
        ):
            symbol = resolved_type.get_def_symbol()
            qn = analysis.get_qualified_name_from_map(symbol)
            cpp_name = "::".join(qn.to_ident_list())
            return f"{cpp_name}::SERIALIZED_SIZE"
        else:
            raise ValueError(f"Unhandled type: {resolved_type}")

    def calculate_internal_port_buffer(
        self,
        port_instance: fpm_port_instance.InternalPortInstance,
        analysis: fpm_Analysis,
    ) -> str:
        """Calculate buffer size expression for internal port by summing parameter sizes

        Raises:
            ValueError: If type not found in type map
            AttributeError: If port structure is invalid
            KeyError: If type lookup fails
        """
        try:
            ast_node = port_instance.a_node
            spec: fpm_fpp_ast.fpp_ast.SpecInternalPort = ast_node[1]
            params = spec.data.params
            if not params:
                return "0"

            size_expressions = []
            for annotated_param in params:
                _, param_node, _ = annotated_param
                formal_param = param_node.data
                logger.debug(
                    f"    param: {formal_param.name}, type: {formal_param.type_name}"
                )
                type_node_id = formal_param.type_name.get_id()

                if type_node_id not in analysis.type_map:
                    raise ValueError(f"Could not find type in type map: {type_node_id}")

                resolved_type = analysis.type_map[type_node_id]
                type_size_expr = self.resolved_type_to_cpp_size_expr(
                    resolved_type, analysis
                )
                logger.debug(
                    f"    type_size_expr: {type_size_expr!r}, resolved_type: {resolved_type!r}"
                )
                size_expressions.append(type_size_expr)

            return " + ".join(size_expressions)

        except (AttributeError, KeyError) as e:
            logger.warning(
                f"Warning: Could not calculate internal port size: {e}\n{port_instance.__dict__!r}"
            )
            raise

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
            "// Auto-generated by priority_buffer_analyzer.py (fprime_python_model)",
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
        description="Analyze F' components for priority buffer sizes using fprime_python_model",
        epilog="Requires FPRIME_ENABLE_JSON_MODEL_GENERATION in CMakeLists.txt",
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
        help="Path to topology JSON files (e.g., build-dir/Deployment/Top)",
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
    except Exception as e:
        logger.error(f"Error: {e}")
        if args.verbose:
            traceback.print_exc()
        return 1


if __name__ == "__main__":
    sys.exit(main())
