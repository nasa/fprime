from __future__ import annotations

import ast
import builtins
from typing import Optional, List, Union, TypeVar, cast
 
from stdlib_list import stdlib_list
 
from mr_proper.common_types import AnyFuncdef
from mr_proper.config import TARGET_PYTHON_VERSION
 
if False:  # TYPE_CHECKING
    from typing import Type
 
 
T = TypeVar('T', bound=ast.AST)
 
 
BUILTINS_LIST = {b for b in dir(builtins) if not b.startswith('_')}
STDLIB_MODULES_NAMES = stdlib_list(TARGET_PYTHON_VERSION)
 
 
def get_ast_tree(pyfilepath: str) -> Optional[ast.Module]:
    with open(pyfilepath, 'r') as file_handler:
        try:
            file_content = file_handler.read()
        except UnicodeDecodeError:
            return None
    try:
        ast_tree = ast.parse(file_content)
    except SyntaxError:
        return None
    set_parents(ast_tree)
    return ast_tree
 
 
def set_parents(ast_tree: ast.AST) -> None:
    for node in ast.walk(ast_tree):
        for child in ast.iter_child_nodes(node):
            child.parent = node  # type: ignore
    ast_tree.parent = None  # type: ignore
 
 
def get_all_funcdefs_from(ast_tree: ast.AST) -> List[AnyFuncdef]:
    return [n for n in ast.walk(ast_tree) if isinstance(n, (ast.FunctionDef, ast.AsyncFunctionDef))]
 
 
def get_nodes_from_funcdef_body(
    funcdef: Union[ast.FunctionDef, ast.AsyncFunctionDef],
    of_types: List[Type[T]],
) -> List[T]:
    nodes: List[ast.AST] = []
    for node in funcdef.body:
        nodes += [n for n in ast.walk(node) if isinstance(n, tuple(of_types))]
    return nodes  # type: ignore
 
 
Function `is_imported_from_stdlib` has a Cognitive Complexity of 6 (exceeds 5 allowed). Consider refactoring.
def is_imported_from_stdlib(name: str, file_ast_tree: ast.Module) -> Optional[bool]:
    for import_node in get_all_global_import_nodes(file_ast_tree):
        for full_import_path in get_full_import_pathes(import_node):
            if full_import_path.split('.')[-1] == name:
                base_module = full_import_path.split('.')[0]
                return base_module in STDLIB_MODULES_NAMES
    return None
 
 
Function `get_local_var_names_from_funcdef` has a Cognitive Complexity of 8 (exceeds 5 allowed). Consider refactoring.
def get_local_var_names_from_funcdef(funcdef_node: AnyFuncdef) -> List[str]:
    local_vars_names: List[str] = []
    for assign_node in get_nodes_from_funcdef_body(funcdef_node, [ast.Assign]):
        for target in assign_node.targets:
            local_vars_names += [n.id for n in ast.walk(target) if isinstance(n, ast.Name)]
    for annassign_node in get_nodes_from_funcdef_body(funcdef_node, [ast.AnnAssign]):
        if isinstance(annassign_node.target, ast.Name):
            local_vars_names.append(annassign_node.target.id)
    for comprehension in get_nodes_from_funcdef_body(funcdef_node, [ast.comprehension, ast.For]):
        comprehension = cast(ast.comprehension, comprehension)
        local_vars_names += get_local_var_names_from_loop(comprehension)
    local_vars_names += {
        n.name
        for n in ast.walk(funcdef_node)
        if isinstance(n, ast.ExceptHandler) and n.name
    }
    return sorted(set(local_vars_names))
 
 
def get_local_var_names_from_loop(loop_node: Union[ast.comprehension, ast.For]) -> List[str]:
    if isinstance(loop_node.target, ast.Name):
        return [loop_node.target.id]
    elif isinstance(loop_node.target, ast.Tuple):
        return [e.id for e in loop_node.target.elts if isinstance(e, ast.Name)]
    return []
 
 
Function `get_all_global_import_nodes` has a Cognitive Complexity of 6 (exceeds 5 allowed). Consider refactoring.
def get_all_global_import_nodes(file_ast_tree: ast.AST) -> List[Union[ast.ImportFrom, ast.Import]]:
    nodes_with_local_scope = {ast.FunctionDef, ast.AsyncFunctionDef, ast.ClassDef}
 
    import_nodes: List[Union[ast.ImportFrom, ast.Import]] = []
    for node in getattr(file_ast_tree, 'body', []):
        if isinstance(node, (ast.ImportFrom, ast.Import)):
            import_nodes.append(node)
        if hasattr(node, 'body') and not isinstance(node, tuple(nodes_with_local_scope)):
            import_nodes += get_all_global_import_nodes(node)
    return import_nodes
 
 
def get_full_import_pathes(import_node: Union[ast.ImportFrom, ast.Import]) -> List[str]:
    if isinstance(import_node, ast.Import):
        return [n.name for n in import_node.names]
    elif isinstance(import_node, ast.ImportFrom):
        return [f'{import_node.module}.{n.name}' for n in import_node.names]"
 

# Import this code to CI and Nasa Team.

Import datacloud & nasacloud
