#!/bin/sh

# ----------------------------------------------------------------------
# hpp.sh
# ----------------------------------------------------------------------

name=$1

echo 'class '$name' :
  public Fw::Serializable
{

  public:

    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    typedef enum {'
awk 'BEGIN { FS="\n"; RS="" }
NR > 1 {
  printf("      %s", $1)
  if ($2 != "" && $2 != "~")
    printf(" = %s", $2)
  printf(",");
  if ($3 != "" && $3 != "~")
    printf(" //!< %s", $3)
  printf("\n")
}' < ${name}EnumAi.txt
echo '    } t;

  public:

    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    enum {
      SERIALIZED_SIZE = sizeof(FwEnumStoreType)
    };

  public:

    // ----------------------------------------------------------------------
    // Constructors
    // ----------------------------------------------------------------------

    //! Construct '$name' object with default initialization
    '$name'(void);

    //! Construct '$name' object and initialize its value
    '$name'(
        const t e //!< The enum value
    );

    //! Copy constructor
    '$name'(
        const '$name'& other //!< The other object
    );

  public:

    // ----------------------------------------------------------------------
    // Operators
    // ----------------------------------------------------------------------

    //! Assignment operator
    const '$name'& operator=(
        const '$name'& other //!< The other object
    );

    //! Assignment operator
    const '$name'& operator=(
        const NATIVE_INT_TYPE a //!< The integer to copy
    );

    //! Assignment operator
    const '$name'& operator=(
        const NATIVE_UINT_TYPE a //!< The integer to copy
    );

    //! Equality operator
    bool operator==(
        const '$name'& other //!< The other object
    ) const;

    //! Inequality operator
    bool operator!=(
        const '$name'& other //!< The other object
    ) const;

#ifdef BUILD_UT
    //! Ostream operator for '$name' object
    friend std::ostream& operator<<(
        std::ostream& os, //!< The ostream
        const '$name'& obj //!< The object
    );
#endif

  public:

    // ----------------------------------------------------------------------
    // Methods
    // ----------------------------------------------------------------------

    //! Serialization
    Fw::SerializeStatus serialize(
        Fw::SerializeBufferBase& buffer //!< The serial buffer
    ) const;

    //! Deserialization
    Fw::SerializeStatus deserialize(
        Fw::SerializeBufferBase& buffer //!< The serial buffer
    );

  public:

    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The enumeration value
    t e;

};'
