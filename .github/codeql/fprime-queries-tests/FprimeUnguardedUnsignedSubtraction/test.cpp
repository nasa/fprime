namespace Fw {
void SwAssert(int line);
}
#define FW_ASSERT(cond) ((cond) ? static_cast<void>(0) : Fw::SwAssert(__LINE__))

typedef unsigned long FwSizeType;
typedef unsigned char U8;

// Minimal stand-ins for the F Prime buffer/container shapes.
class Buffer {
  public:
    FwSizeType getSize() const;
    U8* getData() const;
};

class Container {
  public:
    static const FwSizeType MIN_PACKET_SIZE = 40;
    static const FwSizeType DATA_OFFSET = 24;
    FwSizeType getDataSize() const;
};

void consume(U8* data, FwSizeType size);

// Violation of cpp/fprime/unguarded-unsigned-subtraction (CWE-191):
// the buffer size comes from the caller and is never checked against
// MIN_PACKET_SIZE, so a short buffer underflows to near SIZE_MAX.
// This is the shape fixed in nasa/fprime#5518.
void unguarded(const Buffer& buf) {
    consume(buf.getData(), buf.getSize() - Container::MIN_PACKET_SIZE);
}

// Compliant: FW_ASSERT is a valid check in F Prime. Projects choose their
// own assert level; a site that asserts the invariant is not unchecked.
void checkedByAssert(const Buffer& buf) {
    FW_ASSERT(buf.getSize() >= Container::MIN_PACKET_SIZE);
    consume(buf.getData(), buf.getSize() - Container::MIN_PACKET_SIZE);
}

// Compliant: an explicit run-time check that survives with assertions
// disabled.
void guardedByEarlyReturn(const Buffer& buf) {
    if (buf.getSize() < Container::MIN_PACKET_SIZE) {
        return;
    }
    consume(buf.getData(), buf.getSize() - Container::MIN_PACKET_SIZE);
}

// Compliant: the same check written the other way round.
void guardedByIf(const Buffer& buf) {
    if (buf.getSize() >= Container::MIN_PACKET_SIZE) {
        consume(buf.getData(), buf.getSize() - Container::MIN_PACKET_SIZE);
    }
}

// Compliant: the check is written on a local copy of the size. This is the
// Fw::DpContainer::setBuffer shape.
void checkedOnLocalCopy(const Buffer& buf) {
    const FwSizeType size = buf.getSize();
    FW_ASSERT(size >= Container::MIN_PACKET_SIZE);
    consume(buf.getData(), buf.getSize() - Container::MIN_PACKET_SIZE);
}

FwSizeType requiredSize();

// Compliant: the bound is computed at run time, so the query cannot show it
// is too small. This is the Svc::FileDownlink::sendFilePacket shape.
void checkedAgainstComputedBound(const Buffer& buf) {
    const FwSizeType needed = requiredSize() + Container::MIN_PACKET_SIZE;
    FW_ASSERT(buf.getSize() >= needed);
    consume(buf.getData(), buf.getSize() - Container::MIN_PACKET_SIZE);
}

// Violation: the only comparison of the size is unrelated to the subtraction
// and far too weak to cover it. This is the shape of the FW_ASSERT that
// follows the subtraction in DpCompressProc; it must keep being reported.
void unrelatedCheckAfterSubtraction(const Buffer& buf) {
    const FwSizeType remaining = buf.getSize() - Container::MIN_PACKET_SIZE;
    FW_ASSERT(buf.getSize() > 0);
    consume(buf.getData(), remaining);
}

// Compliant: `> 0` is the idiomatic guard for a `- 1` subtraction. The
// guard constant (0) is one less than the subtrahend (1), which is
// exactly strong enough. An earlier version of this query compared the
// two constants for equality and reported this, which was a false
// positive -- this case pins the fix.
void guardedByGreaterThanZero(const Buffer& buf) {
    if (buf.getSize() > 0) {
        consume(buf.getData(), buf.getSize() - 1);
    }
}

// Compliant: a guard stronger than required is still a guard.
void guardedByStrongerBound(const Buffer& buf) {
    if (buf.getSize() < Container::MIN_PACKET_SIZE + 8) {
        return;
    }
    consume(buf.getData(), buf.getSize() - Container::MIN_PACKET_SIZE);
}

// Violation: a guard that is too weak to cover the subtraction. Checking
// against 2 says nothing about whether the size reaches MIN_PACKET_SIZE.
void guardedTooWeakly(const Buffer& buf) {
    if (buf.getSize() > 2) {
        consume(buf.getData(), buf.getSize() - Container::MIN_PACKET_SIZE);
    }
}

// Compliant: subtracting a variable rather than a compile-time constant
// is out of scope -- too common and too often intentional.
void variableSubtrahend(const Buffer& buf, FwSizeType n) {
    consume(buf.getData(), buf.getSize() - n);
}

// Compliant: not a size accessor, so the query says nothing about intent.
FwSizeType plainArithmetic(FwSizeType value) {
    return value - Container::MIN_PACKET_SIZE;
}

// Violation: the accessor is a different one from the guarded family,
// and it is unchecked here.
void unguardedDataSize(const Container& c) {
    consume(nullptr, c.getDataSize() - Container::DATA_OFFSET);
}

// Violation: the only size comparison in the function is on a different
// object, so nothing here constrains `buf`. This is the shape that let
// nasa/fprime#5518 through a passing test suite: `procRequest_handler` is
// ~330 lines and compares `compression_buffer` and `data_reser`, never the
// port-supplied `fwBuffer`. The bound is computed at run time, which the
// query must keep accepting for same-object checks -- so only the receiver
// separates this from `checkedAgainstComputedBound` above.
void checkOnOtherObject(const Buffer& buf, const Buffer& other) {
    const FwSizeType limit = requiredSize();
    consume(buf.getData(), buf.getSize() - Container::MIN_PACKET_SIZE);
    FW_ASSERT(other.getSize() <= limit);
}

// Violation: the bound is a constant and is strong enough, but it is
// checked on a different object. Isolates receiver identity from bound
// arithmetic: a fix that only tightened the bound handling passes the case
// above and fails this one.
void checkOnOtherObjectConstantBound(const Buffer& buf, const Buffer& other) {
    FW_ASSERT(other.getSize() >= Container::MIN_PACKET_SIZE);
    consume(buf.getData(), buf.getSize() - Container::MIN_PACKET_SIZE);
}

// Violation: the check is on a local copy of a *different* object's size.
// The receiver has to be carried out through the local-copy form as well as
// the direct call; a fix that matches receivers only on direct calls still
// misses this. DpCompressProc has exactly this second, independent path.
void checkOnLocalCopyOfOtherObject(const Buffer& buf, const Buffer& other) {
    const FwSizeType otherSize = other.getSize();
    consume(buf.getData(), buf.getSize() - Container::MIN_PACKET_SIZE);
    FW_ASSERT(otherSize >= Container::MIN_PACKET_SIZE);
}

// Compliant: `buf` is checked, even though the function also compares a
// different buffer. One sufficient check is enough -- receiver matching
// must not degrade into requiring every comparison to be on the subtracted
// object.
void checkedAmongOtherObjectChecks(const Buffer& buf, const Buffer& other) {
    FW_ASSERT(other.getSize() > 0);
    FW_ASSERT(buf.getSize() >= Container::MIN_PACKET_SIZE);
    consume(buf.getData(), buf.getSize() - Container::MIN_PACKET_SIZE);
}
