# Replace DpCatalog Binary Tree with RedBlackTreeSet

## Context

DpCatalog currently uses a custom binary search tree implementation with manual node management (DpBtreeNode with left/right/parent pointers) and manual free list management. This implementation has O(n) worst-case performance for insertion/lookup and requires significant manual balancing code.

The user has requested replacing this with RedBlackTreeMap from Fw/DataStructures/. After analysis, **I recommend using RedBlackTreeSet instead of RedBlackTreeMap** because:

1. DpStateEntry is stored as a complete unit with comprehensive comparison operators already implemented
2. There's no natural key-value separation - the entire entry IS the data
3. RedBlackTreeSet semantics match the use case: a sorted collection of unique entries
4. Simpler API: `insert(entry)` vs `insert(key, value)`

This change will provide guaranteed O(log n) insert/remove performance, eliminate manual tree balancing code, and reduce complexity while maintaining identical sorting behavior.

## Implementation Strategy

### 1. Data Structure Replacement

**Remove from DpCatalog.hpp (lines 148-154, 248-251):**
```cpp
struct DpBtreeNode {
    DpStateEntry entry;
    DpBtreeNode* left;
    DpBtreeNode* right;
    DpBtreeNode* parent;
};

DpBtreeNode* m_dpTree = nullptr;
DpBtreeNode* m_freeListHead = nullptr;
DpBtreeNode* m_currentNode = nullptr;
DpBtreeNode* m_currentXmitNode = nullptr;
```

**Add to DpCatalog.hpp:**
```cpp
#include <Fw/DataStructures/RedBlackTreeSet.hpp>

// In private data section:
Fw::RedBlackTreeSet<DpStateEntry, DP_MAX_FILES> m_dpCatalog;  // Sorted catalog of DPs
typename Fw::RedBlackTreeSet<DpStateEntry, DP_MAX_FILES>::ConstIterator m_currentIter;
DpStateEntry m_currentXmitEntry;  // Entry currently being transmitted
bool m_hasCurrentXmit = false;    // Whether m_currentXmitEntry is valid
```

### 2. Method Transformations

#### configure() - DpCatalog.cpp:29-90
- Update memory calculation to only allocate state file data (DpDstateFileEntry array)
- RedBlackTreeSet storage is already allocated as member variable
- **Before:** `slotSize = sizeof(DpBtreeNode) + sizeof(DpDstateFileEntry)`
- **After:** `slotSize = sizeof(DpDstateFileEntry)`

#### resetBinaryTree() → resetCatalog() - DpCatalog.cpp:92-114
```cpp
void DpCatalog::resetCatalog() {
    m_dpCatalog.clear();
    m_currentIter = m_dpCatalog.end();
    m_hasCurrentXmit = false;
    m_pendingFiles = 0;
    m_pendingDpBytes = 0;
    m_catalogBuilt = false;
}
```

#### insertEntry() - DpCatalog.cpp:659-694
- Simplify to single API call
- Return bool instead of pointer
- RedBlackTreeSet handles all insertion logic and balancing
```cpp
bool DpCatalog::insertEntry(DpStateEntry& entry) {
    Fw::Success status = m_dpCatalog.insert(entry);
    return (status == Fw::Success::SUCCESS);
}
```

#### Remove methods (no longer needed):
- `allocateNode()` - lines 728-751
- `deallocateNode()` - lines 753-879
- `checkLeftRight()` - lines 696-726

#### findNextTreeNode() → findNextEntry() - DpCatalog.cpp:915-953
- Replace manual tree traversal with iterator-based approach
- Iterator automatically provides in-order traversal
```cpp
bool DpCatalog::findNextEntry(DpStateEntry& entry) {
    if (m_dpCatalog.getSize() == 0) {
        return false;
    }
    if (m_currentIter == m_dpCatalog.end()) {
        m_currentIter = m_dpCatalog.begin();
    }
    if (m_currentIter == m_dpCatalog.end()) {
        return false;
    }
    entry = *m_currentIter;
    ++m_currentIter;
    return true;
}
```

#### sendNextEntry() - DpCatalog.cpp:881-913
- Call `findNextEntry()` instead of `findNextTreeNode()`
- Store entry in `m_currentXmitEntry` instead of pointer to node
- Set `m_hasCurrentXmit = true`

#### fileDone_handler() - DpCatalog.cpp:980-1013
- Use `m_dpCatalog.remove(m_currentXmitEntry)` to remove transmitted entry
- No more `deallocateNode()` call
- Assert removal succeeds: `FW_ASSERT(status == Fw::Success::SUCCESS)`

#### processFile() - DpCatalog.cpp:470-605
- Change `insertEntry()` from returning pointer to returning bool
- Handle runtime additions: if new entry has higher priority than current iterator position, reset iterator to `begin()`
```cpp
if (m_currentIter != m_dpCatalog.end()) {
    const DpStateEntry& currentEntry = *m_currentIter;
    if (entry < currentEntry) {
        m_currentIter = m_dpCatalog.begin();
    }
}
```

#### addToCat_handler() - DpCatalog.cpp:1020-1071
- When restarting transmission after runtime addition: `m_currentIter = m_dpCatalog.begin()`

#### Command handlers - Update all `resetBinaryTree()` calls:
- `doCatalogBuild()` - line 319-364
- `CLEAR_CATALOG_cmdHandler()` - line 1153-1158

### 3. Sorting Behavior Verification

**No changes needed to comparison operators.** DpStateEntry already has correct implementation (DpCatalog.cpp:610-657):
- `compareEntries()` static method with composite key comparison
- `operator<` - uses `compareEntries() < 0`
- `operator==` - uses `compareEntries() == 0`

**Sorting order (maintained):**
1. Priority (lower value = higher priority)
2. Time seconds (older = higher priority)
3. Time subseconds (older = higher priority)
4. ID (lower = higher priority)

RedBlackTreeSet will use these operators directly - no changes needed.

### 4. Memory Management Changes

**Current approach:**
- Single `allocator.allocate()` call for tree nodes + state file data
- Tree nodes placed at beginning, state file data after

**New approach:**
- RedBlackTreeSet storage allocated as member variable (no dynamic allocation)
- Only allocate state file data from allocator
- Memory requirement: `DP_MAX_FILES * sizeof(DpDstateFileEntry)`
- `m_numDpSlots` still used for state file capacity

**Impact:** RedBlackTreeSet uses approximately same memory as custom tree (one additional byte per node for color). Total memory from allocator is reduced by ~50%.

### 5. Iterator Invalidation Handling

**Risk:** Adding/removing during iteration could invalidate iterator

**Mitigation:**
1. **Insertions:** RedBlackTreeSet iterators remain valid after insertions (red-black tree property)
2. **Removals:** Only remove `m_currentXmitEntry` which is already past the iterator position
3. **Runtime additions:** If new entry < current entry, reset iterator to `begin()` (same as current logic)

**Test coverage:** Add unit test for runtime addition during transmission

### 6. Performance Improvements

- **Insert:** O(n) worst case → O(log n) guaranteed
- **Find:** O(n) worst case → O(log n) guaranteed  
- **Remove:** O(n) worst case → O(log n) guaranteed
- **Iteration:** O(n) → O(n) (unchanged)
- **Balancing:** Manual → Automatic

No performance degradation expected.

## Critical Files

### Implementation Files
1. **Svc/DpCatalog/DpCatalog.hpp** - Add include, remove DpBtreeNode struct, replace member variables
2. **Svc/DpCatalog/DpCatalog.cpp** - Implement all method changes (~500 line changes)

### Test Files
3. **Svc/DpCatalog/test/ut/DpCatalogTester.cpp** - Update `testTree()` method, add iterator tests
4. **Svc/DpCatalog/test/ut/DpCatalogTester.hpp** - Update test helper if needed

### Reference Files (read-only)
5. **Fw/DataStructures/RedBlackTreeSet.hpp** - API reference
6. **config/DpCatalogCfg.hpp** - DP_MAX_FILES constant reference
7. **.github/agents/fprime-code-review.agent.md** - Coding guidelines

## Verification Steps

### Unit Tests
1. **Update existing tests:**
   - `testTree()` - Verify insertion, removal, iteration with RedBlackTreeSet
   - Test insertion order independence
   - Test capacity limits (fill to DP_MAX_FILES)

2. **Add new tests:**
   - Iterator behavior (begin/end, increment, order verification)
   - Runtime addition during transmission (iterator reset)
   - Removal during iteration
   - Duplicate entry handling (insert updates existing)

### Integration Tests
1. Run existing integration tests in `Svc/DpCatalog/test/int/`
2. Verify transmission order matches priority order
3. Verify state file persistence across catalog rebuilds
4. Test runtime DP additions via `addToCat` during transmission

### Build and Run
```bash
cd /home/tcanham/source/fprime-nasa
fprime-util generate --ut
cd Svc/DpCatalog
fprime-util check
```

### Behavioral Verification
1. Build catalog with multiple DPs of varying priorities
2. Verify highest priority (lowest number, oldest time) transmits first
3. Add DP during transmission, verify it's inserted in correct priority order
4. Stop and restart transmission, verify state persists correctly

## Edge Cases

1. Empty catalog transmission - iterator `begin() == end()`
2. Single entry catalog - verify iteration works
3. Catalog full - `insert()` returns `FAILURE`
4. Transmission interrupted - iterator state preserved
5. Runtime addition with higher priority - iterator resets to `begin()`
6. Duplicate DPs - `insert()` updates existing entry (improved behavior vs current)

## Coding Guidelines Compliance

Following `.github/agents/fprime-code-review.agent.md`:
- ✓ C++14 compliant, no lambdas, no STL
- ✓ No dynamic memory after initialization (RedBlackTreeSet uses member storage)
- ✓ Use `Fw::` types (RedBlackTreeSet is in Fw namespace)
- ✓ Use `nullptr` (not NULL)
- ✓ Initialize all variables (`m_hasCurrentXmit = false`)
- ✓ `FW_ASSERT` only for programming errors (used for removal success check)
- ✓ Prefer `Fw/DataStructures` types over custom (this is the goal!)
- ✓ Follow Rule of Three/Five (RedBlackTreeSet handles this)
- ✓ Unit tests required (will update existing + add new)
- ✓ Update SDD if exists

## Migration Sequence

**Phase 1: Header changes**
1. Add `#include <Fw/DataStructures/RedBlackTreeSet.hpp>`
2. Remove `DpBtreeNode` struct
3. Replace member variables
4. Update method signatures (`insertEntry()` return type, `findNextEntry()` new method)

**Phase 2: Implementation**
1. Update `configure()` memory allocation
2. Implement `resetCatalog()` (replace `resetBinaryTree()`)
3. Replace `insertEntry()` implementation
4. Remove `allocateNode()`, `deallocateNode()`, `checkLeftRight()`
5. Implement `findNextEntry()` (replace `findNextTreeNode()`)
6. Update `sendNextEntry()`
7. Update `fileDone_handler()`
8. Update `processFile()`
9. Update `addToCat_handler()`
10. Update command handlers

**Phase 3: Testing**
1. Update unit tests
2. Run test suite
3. Fix any issues
4. Integration testing

**Phase 4: Documentation**
1. Update inline comments ("binary tree" → "catalog")
2. Update SDD if exists
3. Update `configure()` documentation

## Risk Assessment

**Low Risk:**
- DpStateEntry comparison operators already correct
- RedBlackTreeSet is well-tested F Prime component
- Iterator-based traversal is standard pattern
- Memory usage similar to current implementation

**Mitigation:**
- Comprehensive unit test coverage
- Integration test verification
- Behavioral testing with various DP priorities
- Code review per F Prime guidelines
