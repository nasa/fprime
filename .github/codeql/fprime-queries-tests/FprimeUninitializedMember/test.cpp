typedef unsigned int U32;
typedef float F32;

enum Mode { OFF, ON };

class Helper {
  public:
    Helper() : m_value(0) {}  // Compliant
  private:
    U32 m_value;
};

// Violation of cpp/fprime/uninitialized-member: m_count and m_mode
class Bad {
  public:
    Bad() : m_ptr(nullptr) {}

  private:
    U32 m_count;
    Mode m_mode;
    U32* m_ptr;
    Helper m_helper;  // class type: not flagged
};

// Compliant: initializer list, in-class initializer, body assignment, helper
class Good {
  public:
    Good() : m_count(0) {
        m_mode = OFF;
        this->reset();
    }
    void reset() { m_buffer[0] = 0; }

  private:
    U32 m_count;
    Mode m_mode;
    U32* m_ptr = nullptr;
    F32 m_buffer[4];
    static U32 s_instances;  // static: not flagged
};

// Compliant: copy constructor is not checked
class Copyable {
  public:
    Copyable() : m_a(0), m_b(0) {}
    Copyable(const Copyable& other) { m_a = other.m_a; }

  private:
    U32 m_a;
    U32 m_b;
};

// Violation of cpp/fprime/uninitialized-member: m_arr left indeterminate
class BadArray {
  public:
    BadArray() {}

  private:
    U32 m_arr[8];
};

void fillBuffer(U32* dest, U32 size);

// Compliant: array handed to a fill routine (decays to pointer)
class Filled {
  public:
    Filled() { fillBuffer(m_data, 4); }

  private:
    U32 m_data[4];
};
