typedef unsigned int U32;
namespace Fw {
void SwAssert(const char* file, U32 line);
}
#define FW_ASSERT(cond, ...) ((cond) ? (void)0 : Fw::SwAssert(__FILE__, __LINE__))

const U32 SIZE = 8;

class Table {
  public:
    U32 lookup(U32 key);
    void fill();
    U32 byParam(U32 index);
    U32 fromMember();
    U32 viaPointer(U32* data);

  private:
    U32 m_entries[SIZE];
    U32 m_cursor;
};

U32 Table::lookup(U32 key) {
    U32 slot = key * 2;
    return this->m_entries[slot];  // Violation of cpp/fprime/unchecked-array-index
}

void Table::fill() {
    for (U32 i = 0; i < SIZE; i++) {
        this->m_entries[i] = i;  // Compliant: loop-bounded
    }
    this->m_entries[0] = 1;  // Compliant: constant index
}

U32 Table::byParam(U32 index) {
    return this->m_entries[index];  // Compliant here: parameters are cpp/fprime/checking-parameter-values
}

U32 Table::fromMember() {
    U32 a = this->m_entries[this->m_cursor];  // Violation of cpp/fprime/unchecked-array-index
    FW_ASSERT(this->m_cursor < SIZE, this->m_cursor);
    U32 b = this->m_entries[this->m_cursor];  // Compliant: asserted
    U32 c = 0;
    if (this->m_cursor < SIZE) {
        c = this->m_entries[this->m_cursor];  // Compliant: checked
    }
    return a + b + c;
}

U32 Table::viaPointer(U32* data) {
    U32 slot = this->m_cursor;
    return data[slot];  // Compliant: pointer base has no static bound (out of scope)
}
