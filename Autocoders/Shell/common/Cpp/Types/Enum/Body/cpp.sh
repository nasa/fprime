#!/bin/sh

# ----------------------------------------------------------------------
# cpp.sh
# ----------------------------------------------------------------------

name=$1

echo '// ----------------------------------------------------------------------
// Constructors
// ----------------------------------------------------------------------

'$name' ::
  '$name'(void) :
    Serializable()
{
  this->e = (t)0;
}

'$name' ::
  '$name'(const t e) :
    Serializable()
{
  this->e = e;
}

'$name' ::
  '$name'(const '$name'& other) :
    Serializable()
{
  this->e = other.e;
}

// ----------------------------------------------------------------------
// Instance methods
// ----------------------------------------------------------------------

const '$name'& '$name' ::
  operator=(const '$name'& other)
{
  this->e = other.e;
  return *this;
}

const '$name'& '$name' ::
  operator=(const NATIVE_INT_TYPE a)
{
  this->e = static_cast<t>(a);
  return *this;
}

const '$name'& '$name' ::
  operator=(const NATIVE_UINT_TYPE a)
{
  this->e = static_cast<t>(a);
  return *this;
}

bool '$name' ::
  operator==(const '$name'& other) const
{
  return this->e == other.e;
}

bool '$name' ::
  operator!=(const '$name'& other) const
{
  return !(*this == other);
}

#ifdef BUILD_UT
std::ostream& operator<<(
    std::ostream& os,
    const '$name'& obj
) {
  os << "'$name'::";
  const '$name'::t e = obj.e;
  switch (e) {'
awk 'BEGIN { FS="\n"; RS="" }
NR > 1 {
  print "  case '$name'::" $1 ":"
  print "    os << \"" $1 "\";"
  print "    break;"
}' < ${name}EnumAi.txt
echo '  default:
    os << "[invalid]";
    break;
  }
  os << " (" << e << ")";
  return os;
}
#endif

Fw::SerializeStatus '$name' ::
  serialize(Fw::SerializeBufferBase& buffer) const
{
  Fw::SerializeStatus status;
  status = buffer.serialize(static_cast<FwEnumStoreType>(this->e));
  return status;
}

Fw::SerializeStatus '$name' ::
  deserialize(Fw::SerializeBufferBase& buffer)
{
  Fw::SerializeStatus status;
  FwEnumStoreType es;
  status = buffer.deserialize(es);
  if (status == Fw::FW_SERIALIZE_OK)
    this->e = static_cast<t>(es);
  return status;
}'
