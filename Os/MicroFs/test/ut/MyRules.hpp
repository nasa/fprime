

    // ------------------------------------------------------------------------------------------------------
    // Rule:  InitFileSystem
    //
    // ------------------------------------------------------------------------------------------------------
    struct InitFileSystem : public STest::Rule<Os::Tester> {

            // ----------------------------------------------------------------------
            // Construction
            // ----------------------------------------------------------------------

            //! Constructor
            InitFileSystem(U32 numBins, U32 fileSize, U32 numFiles);

            // ----------------------------------------------------------------------
            // Public member functions
            // ----------------------------------------------------------------------

            //! Precondition
            bool precondition(
                const Os::Tester& state //!< The test state
            );

            //! Action
            void action(
                Os::Tester& state //!< The test state
            );

            U32 numBins;
            U32 fileSize;
            U32 numFiles;


    };

    



    // ------------------------------------------------------------------------------------------------------
    // Rule:  OpenFile
    //
    // ------------------------------------------------------------------------------------------------------
    struct OpenFile : public STest::Rule<Os::Tester> {

            // ----------------------------------------------------------------------
            // Construction
            // ----------------------------------------------------------------------

            //! Constructor
            OpenFile(const char *filename);

            // ----------------------------------------------------------------------
            // Public member functions
            // ----------------------------------------------------------------------

            //! Precondition
            bool precondition(
                const Os::Tester& state //!< The test state
            );

            //! Action
            void action(
                Os::Tester& state //!< The test state
            );

            const char *filename;


    };

    



    // ------------------------------------------------------------------------------------------------------
    // Rule:  Cleanup
    //
    // ------------------------------------------------------------------------------------------------------
    struct Cleanup : public STest::Rule<Os::Tester> {

            // ----------------------------------------------------------------------
            // Construction
            // ----------------------------------------------------------------------

            //! Constructor
            Cleanup();

            // ----------------------------------------------------------------------
            // Public member functions
            // ----------------------------------------------------------------------

            //! Precondition
            bool precondition(
                const Os::Tester& state //!< The test state
            );

            //! Action
            void action(
                Os::Tester& state //!< The test state
            );

    };

    



    // ------------------------------------------------------------------------------------------------------
    // Rule:  WriteData
    //
    // ------------------------------------------------------------------------------------------------------
    struct WriteData : public STest::Rule<Os::Tester> {

            // ----------------------------------------------------------------------
            // Construction
            // ----------------------------------------------------------------------

            //! Constructor
            WriteData(const char *filename, NATIVE_INT_TYPE size, U8 value);

            // ----------------------------------------------------------------------
            // Public member functions
            // ----------------------------------------------------------------------

            //! Precondition
            bool precondition(
                const Os::Tester& state //!< The test state
            );

            //! Action
            void action(
                Os::Tester& state //!< The test state
            );

            NATIVE_INT_TYPE size;
            U8 value;
            const char* filename;

    };

    



    // ------------------------------------------------------------------------------------------------------
    // Rule:  ReadData
    //
    // ------------------------------------------------------------------------------------------------------
    struct ReadData : public STest::Rule<Os::Tester> {

            // ----------------------------------------------------------------------
            // Construction
            // ----------------------------------------------------------------------

            //! Constructor
            ReadData(const char *filename, NATIVE_INT_TYPE size);

            // ----------------------------------------------------------------------
            // Public member functions
            // ----------------------------------------------------------------------

            //! Precondition
            bool precondition(
                const Os::Tester& state //!< The test state
            );

            //! Action
            void action(
                Os::Tester& state //!< The test state
            );

            NATIVE_INT_TYPE size;
            const char *filename;

    };

    



    // ------------------------------------------------------------------------------------------------------
    // Rule:  ResetFile
    //
    // ------------------------------------------------------------------------------------------------------
    struct ResetFile : public STest::Rule<Os::Tester> {

            // ----------------------------------------------------------------------
            // Construction
            // ----------------------------------------------------------------------

            //! Constructor
            ResetFile(const char *filename);

            // ----------------------------------------------------------------------
            // Public member functions
            // ----------------------------------------------------------------------

            //! Precondition
            bool precondition(
                const Os::Tester& state //!< The test state
            );

            //! Action
            void action(
                Os::Tester& state //!< The test state
            );

            const char *filename;

    };

    



    // ------------------------------------------------------------------------------------------------------
    // Rule:  CloseFile
    //
    // ------------------------------------------------------------------------------------------------------
    struct CloseFile : public STest::Rule<Os::Tester> {

            // ----------------------------------------------------------------------
            // Construction
            // ----------------------------------------------------------------------

            //! Constructor
            CloseFile(const char *filename);

            // ----------------------------------------------------------------------
            // Public member functions
            // ----------------------------------------------------------------------

            //! Precondition
            bool precondition(
                const Os::Tester& state //!< The test state
            );

            //! Action
            void action(
                Os::Tester& state //!< The test state
            );

            const char *filename;

    };

    



    // ------------------------------------------------------------------------------------------------------
    // Rule:  Listings
    //
    // ------------------------------------------------------------------------------------------------------
    struct Listings : public STest::Rule<Os::Tester> {

            // ----------------------------------------------------------------------
            // Construction
            // ----------------------------------------------------------------------

            //! Constructor
            Listings(U16 numBins, U16 numFiles);

            // ----------------------------------------------------------------------
            // Public member functions
            // ----------------------------------------------------------------------

            //! Precondition
            bool precondition(
                const Os::Tester& state //!< The test state
            );

            //! Action
            void action(
                Os::Tester& state //!< The test state
            );

            U16 numBins;
            U16 numFiles;

    };

    // ------------------------------------------------------------------------------------------------------
    // Rule:  FreeSpace
    //
    // ------------------------------------------------------------------------------------------------------
    struct FreeSpace : public STest::Rule<Os::Tester> {

            // ----------------------------------------------------------------------
            // Construction
            // ----------------------------------------------------------------------

            //! Constructor
            FreeSpace();

            // ----------------------------------------------------------------------
            // Public member functions
            // ----------------------------------------------------------------------

            //! Precondition
            bool precondition(
                const Os::Tester& state //!< The test state
            );

            //! Action
            void action(
                Os::Tester& state //!< The test state
            );

    };
    