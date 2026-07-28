void doWork(int value);

// Violations of cpp/fprime/for-header-flow-control: side effects in the for
// header unrelated to loop flow control.
void headerSideEffects(int n) {
    int total = 0;
    for (int i = 0; i < n; i++, total += i) {
        doWork(i);
    }

    for (int i = 0; i < n; doWork(i), i++) {
    }
}

class Iterator {
  public:
    Iterator& operator++();
    bool operator!=(const Iterator& other) const;
    int operator*() const;
};

class Container {
  public:
    Iterator begin() const;
    Iterator end() const;
};

// Compliant: overloaded operator++ on the loop-control iterator.
void iteratorLoop(const Container& c) {
    for (Iterator it = c.begin(); it != c.end(); ++it) {
        doWork(*it);
    }
}

// Compliant: header only concerns flow control.
void cleanHeader(int n) {
    int total = 0;
    for (int i = 0; i < n; i++) {
        total += i;
        doWork(total);
    }
}
