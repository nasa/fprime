// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.


#ifndef ACTIVETEXTLOGGERIMPL_HPP_
#define ACTIVETEXTLOGGERIMPL_HPP_

#include <Svc/ActiveTextLogger/ActiveTextLoggerComponentAc.hpp>
#include <Svc/ActiveTextLogger/LogFile.hpp>


namespace Svc {

    //! \class ActiveTextLoggerComponent
    //! \brief Active text logger component class
    //!
    //! Similarly to the PassiveTextLogger, this component takes log texts
    //! and prints them to the console, but does so from a thread to keep
    //! consistent ordering.  It also provides the option to write the text
    //! to a file as well.

    class ActiveTextLogger: public ActiveTextLoggerComponentBase {

        public:

            //!  \brief Component constructor
            //!
            //!  The constructor initializes the state of the component.
            //!
            //!  Note: Making constructor explicit to prevent implicit
            //!  type conversion.
            //!
            //!  \param compName the component instance name
            explicit ActiveTextLogger(const char* compName);

            //!  \brief Component destructor
            //!
            virtual ~ActiveTextLogger(); //!< destructor

            //!  \brief Set log file and max size
            //!
            //!  This is to create an optional log file to write all the messages to.
            //!  The file will not be written to once the max size is hit.
            //!
            //!  \param fileName The name of the file to create.  Must be less than 80 characters.
            //!  \param maxSize The max size of the file
            //!  \param maxBackups The maximum backups of the log file. Default: 10
            //!
            //!  \return true if creating the file was successful, false otherwise
            bool set_log_file(const char* fileName, const U32 maxSize, const U32 maxBackups = 10);


        PRIVATE:

        // ----------------------------------------------------------------------
        // Prohibit Copying
        // ----------------------------------------------------------------------

        /*! \brief Copy constructor
         *
         */
        ActiveTextLogger(const ActiveTextLogger&);

        /*! \brief Copy assignment operator
         *
         */
        ActiveTextLogger& operator=(const ActiveTextLogger&);

        // ----------------------------------------------------------------------
        // Constants/Types
        // ----------------------------------------------------------------------

        // ----------------------------------------------------------------------
        // Member Functions
        // ----------------------------------------------------------------------

        // ----------------------------------------------------------------------
        // Handlers to implement for typed input ports
        // ----------------------------------------------------------------------

        //! Handler for input port TextLogger
        //
        virtual void TextLogger_handler(
            NATIVE_INT_TYPE portNum, /*!< The port number*/
            FwEventIdType id, /*!< Log ID*/
            Fw::Time &timeTag, /*!< Time Tag*/
            const Fw::LogSeverity& severity, /*!< The severity argument*/
            Fw::TextLogString &text /*!< Text of log message*/
        );

        // ----------------------------------------------------------------------
        // Internal interface handlers
        // ----------------------------------------------------------------------

        //! Internal Interface handler for TextQueue
        //!
        virtual void TextQueue_internalInterfaceHandler(
            const Fw::InternalInterfaceString& text /*!< The text string*/
        );

        // ----------------------------------------------------------------------
        // Member Variables
        // ----------------------------------------------------------------------

        // The optional file to text logs to:
        LogFile m_log_file;

    };

}
#endif /* ACTIVETEXTLOGGERIMPL_HPP_ */
