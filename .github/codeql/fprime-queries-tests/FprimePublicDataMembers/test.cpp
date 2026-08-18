// Violation of cpp/fprime/public-data-member: public data member in a class
// with behavior.
class Telemetry {
  public:
    int m_count;

    void update();

  private:
    int m_hidden;
};

// Compliant: plain aggregate with no member functions.
struct Packet {
    int id;
    int length;
};

// Compliant: declared with the struct keyword, even with member functions.
struct Record {
    int value;

    void reset();
};

// Compliant: data members are private.
class Encapsulated {
  public:
    void update();

  private:
    int m_count;
};
