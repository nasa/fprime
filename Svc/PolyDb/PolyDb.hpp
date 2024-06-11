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
#ifndef POLYDB_HPP_
#define POLYDB_HPP_

#include <Svc/PolyDb/PolyDbComponentAc.hpp>
#include <Fw/Types/PolyType.hpp>

namespace Svc {

    //! \class PolyDb
    //! \brief PolyDb Component Class
    //!
    //! This component allows the setting and retrieving of PolyType
    //! telemetry values. It be used as a central analog database
    //! that can decouple measurement sources from measurement users.
    //! The intent is that measurement sources would convert DNs (data numbers)
    //! to ENs (Engineering Numbers) to decouple the conversion as well.
    //!

    class PolyDb : public PolyDbComponentBase {
    public:
        //!  \brief PolyDbImpl constructor
        //!
        //!  The constructor initializes the database to "MeasurementStatus::STALE."
        //!  All values retrieved will have this status until the first
        //!  update is received.
        //!

        explicit PolyDb(const char* const name);

        //!  \brief PolyDbImpl destructor
        //!
        //!  The destructor is empty.
        //!

        virtual ~PolyDb();
    protected:
    private:

        //!  \brief The value getter port handler
        //!
        //!  The getter port handler looks up the indicated entry
        //!  in the database and copies the contents into the user
        //!  supplied arguments status, time, and val.
        //!
        //!  \param portNum port number of request (always 0)
        //!  \param entry entry to retrieve
        //!  \param status last status of retrieved measurement
        //!  \param time time tag of latest measurement
        //!  \param val value of latest measurement

        void getValue_handler(
            NATIVE_INT_TYPE portNum, //!< The port number
            const Svc::PolyDbCfg::PolyDbEntry& entry, //!< The entry to access
            Svc::MeasurementStatus& status, //!< The command response argument
            Fw::Time& time, //!< The time of the measurement
            Fw::PolyType& val //!< The value to be passed
        ) override;

        //!  \brief The value setter port handler
        //!
        //!  The setter port handler takes the values passed
        //!  and updates the entry in the database
        //!
        //!  \param portNum port number of request (always 0)
        //!  \param entry entry to retrieve
        //!  \param status status of new measurement
        //!  \param time time tag of new measurement
        //!  \param val value of new measurement

        void setValue_handler(
            NATIVE_INT_TYPE portNum, //!< The port number
            const Svc::PolyDbCfg::PolyDbEntry& entry, //!< The entry to access
            Svc::MeasurementStatus& status, //!< The command response argument
            Fw::Time& time, //!< The time of the measurement
            Fw::PolyType& val //!< The value to be passed
        ) override;

        //! \struct t_dbStruct
        //! \brief PolyDb database structure
        //!
        //! This structure stores the latest values of the measurements.
        //! The statuses are all initialized to MeasurementStatus::STALE by the constructor.
        //!

        struct t_dbStruct {
            MeasurementStatus status; //!< last status of measurement
            Fw::PolyType val; //!< the last value of the measurement
            Fw::Time time; //!< the timetag of the last measurement
        } m_db[Svc::PolyDbCfg::PolyDbEntry::NUM_CONSTANTS];

    };
}

#endif /* POLYDB_HPP_ */
