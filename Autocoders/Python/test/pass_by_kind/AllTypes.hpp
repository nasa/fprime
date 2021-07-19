/*
 * AllTypes.hpp
 *
 *  Created on: Thursday, 15 July 2021
 *  Author:     dhesikan
 *
 */

namespace Example {

    class AllTypes {
        public:
            AllTypes(bool is_async); //!< constructor with argument async/sync boolean flag
            virtual ~AllTypes(void); //!< destructor
            void checkAsserts(void); //!< function checks output of arguments is expected

            // Declaration of arguments
            U32* arg1;
            U32* arg2;
            U32* arg3;
            U32* arg4;
            ExampleType* arg5;
            ExampleType* arg6;
            ExampleType* arg7;
            ExampleType* arg8;
            Arg9String* arg9;
            Arg10String* arg10;
            Arg11String* arg11;
            Arg12String* arg12;

        private:
            // Stores whether arguments are passed to async or sync port
            bool is_async;

    };

} // end namespace Example
