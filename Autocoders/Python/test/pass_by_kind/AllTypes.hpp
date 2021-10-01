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
            virtual ~AllTypes(); //!< destructor
            void checkAsserts(); //!< function checks output of arguments is expected

            // Declaration of arguments
            U32 arg1;
            U32 arg2;
            U32 arg3;
            ExampleType arg4;
            ExampleType arg5;
            ExampleType arg6;
            Arg7String arg7;
            Arg8String arg8;
            Arg9String arg9;

        private:
            // Stores whether arguments are passed to async or sync port
            bool is_async;

    };

} // end namespace Example
