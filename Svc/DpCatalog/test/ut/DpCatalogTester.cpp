// ======================================================================
// \title  DpCatalogTester.cpp
// \author tcanham
// \brief  cpp file for DpCatalog component test harness implementation class
// ======================================================================

#include "DpCatalogTester.hpp"
#include "Fw/Types/MallocAllocator.hpp"

namespace Svc {

    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    DpCatalogTester ::
        DpCatalogTester() :
        DpCatalogGTestBase("DpCatalogTester", DpCatalogTester::MAX_HISTORY_SIZE),
        component("DpCatalog")
    {
        this->initComponents();
        this->connectPorts();
    }

    DpCatalogTester ::
        ~DpCatalogTester()
    {

    }

    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void DpCatalogTester ::
        doInit()
    {
        Fw::MallocAllocator alloc;

        Fw::String dirs[2];
        dirs[0] = "dir0";
        dirs[1] = "dir1";
        this->component.configure(10,dirs,FW_NUM_ARRAY_ELEMENTS(dirs),100,alloc);
        this->component.shutdown();
    }

    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    Svc::SendFileResponse DpCatalogTester ::
        from_fileOut_handler(
            NATIVE_INT_TYPE portNum,
            const Svc::SendFileRequestPortStrings::StringSize100& sourceFileName,
            const Svc::SendFileRequestPortStrings::StringSize100& destFileName,
            U32 offset,
            U32 length
        )
    {


        return Svc::SendFileResponse();
    }

    void DpCatalogTester ::
        from_pingOut_handler(
            NATIVE_INT_TYPE portNum,
            U32 key
        )
    {
        // TODO
    }

}
