/**
 * \file
 * \author T. Canham
 * \brief PolyDb is a database for storing telemetry for internal software use
 *
 * \copyright
 * Copyright 2009-2015, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 * <br /><br />
 */
#ifndef POLYDBIMPL_HPP_
#define POLYDBIMPL_HPP_

#include <Svc/PolyDb/PolyDbComponentAc.hpp>
#include <Fw/Types/PolyType.hpp>
#include <Svc/PolyDb/PolyDbImplCfg.hpp>

namespace Svc {

    //! \class PolyDbImpl
    //! \brief PolyDb Component Class
    //!
    //! This component allows the setting and retrieving of PolyType
    //! telemetry values. It be used as a central analog database
    //! that can decouple measurement sources from measurement users.
    //! The intent is that measurement sources would convert DNs (data numbers)
    //! to ENs (Engineering Numbers) to decouple the conversion as well.
    //!

    class PolyDbImpl : public PolyDbComponentBase {
        public:
            //!  \brief PolyDbImpl constructor
            //!
            //!  The constructor initializes the database to "MEASUREMENT_STALE."
            //!  All values retrieved will have this status until the first
            //!  update is received.
            //!

#if FW_OBJECT_NAMES == 1
            PolyDbImpl(const char* name);
#else
            PolyDbImpl();
#endif
            //!  \brief PolyDbImpl initialization
            //!
            //!  The PolyDbImpl initialization function calls the base
            //!  class initializer.
            //!

            void init(NATIVE_INT_TYPE instance);

            //!  \brief PolyDbImpl destructor
            //!
            //!  The destructor is empty.
            //!

            virtual ~PolyDbImpl();
        protected:
        private:

            //!  \brief The value getter port handler
            //!
            //!  The getter port handler looks up the indicated entry
            //!  in the database and copies the contents into the user
            //!  supplied arguments status, time, and val.
            //!
            //!  \param portNum port number of request (always 0)
            //!  \param status last status of retrieved measurement
            //!  \param time time tag of latest measurement
            //!  \param val value of latest measurement

            void getValue_handler(NATIVE_INT_TYPE portNum, U32 entry, MeasurementStatus &status, Fw::Time &time, Fw::PolyType &val);

            //!  \brief The value setter port handler
            //!
            //!  The setter port handler takes the values passed
            //!  and updates the entry in the database
            //!
            //!  \param portNum port number of request (always 0)
            //!  \param status status of new measurement
            //!  \param time time tag of new measurement
            //!  \param val value of new measurement

            void setValue_handler(NATIVE_INT_TYPE portNum, U32 entry, MeasurementStatus &status, Fw::Time &time, Fw::PolyType &val);

            //! \struct t_dbStruct
            //! \brief PolyDb database structure
            //!
            //! This structure stores the latest values of the measurements.
            //! The statuses are all initialized to MEASUREMENT_STALE by the constructor.
            //!

            struct t_dbStruct {
                MeasurementStatus status; //!< last status of measurement
                Fw::PolyType val; //!< the last value of the measurement
                Fw::Time time; //!< the timetag of the last measurement
            } m_db[POLYDB_NUM_DB_ENTRIES];

    };
}

#endif /* POLYDBIMPL_HPP_ */
