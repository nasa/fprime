// ======================================================================
// \title  DpCatalogTester.cpp
// \author tcanham
// \brief  cpp file for DpCatalog component test harness implementation class
// ======================================================================

#include "DpCatalogTester.hpp"
#include "Fw/Types/MallocAllocator.hpp"
#include "Fw/Dp/DpContainer.hpp"
#include "Os/File.hpp"
#include "Os/FileSystem.hpp"
#include "Fw/Test/UnitTest.hpp"

namespace Svc {

    static const char* DP_DIR = "./DpTest";

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

    //! Read one DP test
    void DpCatalogTester::readOneDp() {
        Fw::Time time(1000,10000);

        // make a directory for the files
        this->makeDpDir(DP_DIR);

        // clean up last DP
        this->delDp(
            0x123,
            time,
            DP_DIR
        );

        this->genDP(
            0x123,
            10,
            time,
            100,
            Fw::DpState::UNTRANSMITTED,
            false,
            DP_DIR
            );

        Fw::MallocAllocator alloc;

        Fw::String dirs[1];
        dirs[0] = DP_DIR;
        this->component.configure(10,dirs,FW_NUM_ARRAY_ELEMENTS(dirs),100,alloc);

        this->sendCmd_BUILD_CATALOG(0,10);
        this->component.doDispatch();
        this->sendCmd_START_XMIT_CATALOG(0,0,Fw::Wait::NO_WAIT);
        this->component.doDispatch();

        this->component.shutdown();

    }


    void DpCatalogTester::genDP(
        FwDpIdType id,
        FwDpPriorityType prio,
        Fw::Time& time,
        FwSizeType dataSize,
        Fw::DpState dpState,
        bool hdrHashError,
        const char* dir
    ) {
        // Fill DP container
        U8 hdrData[Fw::DpContainer::MIN_PACKET_SIZE];
        Fw::Buffer hdrBuffer(hdrData,Fw::DpContainer::MIN_PACKET_SIZE);
        Fw::DpContainer cont(id,hdrBuffer);
        cont.setPriority(prio);
        cont.setTimeTag(time);
        cont.setDpState(dpState);
        cont.setDataSize(dataSize);
        // serialize file data
        cont.serializeHeader();
        // fill data with ramp
        U8 dpData[dataSize];
        for (FwIndexType byte = 0; byte < static_cast<FwIndexType>(dataSize); byte++) {
            dpData[byte] = byte;
        }
        // open file to write data
        Fw::String fileName;
        fileName.format(Svc::DP_FILENAME_FORMAT,dir,id,time.getSeconds(),time.getUSeconds());
        COMMENT(fileName.toChar());
        Os::File dpFile;
        Os::File::Status stat = dpFile.open(fileName.toChar(),Os::File::Mode::OPEN_CREATE);
        if (stat != Os::File::Status::OP_OK) {
            printf("Error opening file %s: status: %d\n",fileName.toChar(),stat);
            return;
        }
        NATIVE_INT_TYPE size = Fw::DpContainer::Header::SIZE;
        stat = dpFile.write(hdrData,size);
        if (stat != Os::File::Status::OP_OK) {
            printf("Error writing DP file header %s: status: %d\n",fileName.toChar(),stat);
            return;
        }
        if (static_cast<FwSizeType>(size) != Fw::DpContainer::Header::SIZE) {
            printf("Dp file header %s write size didn't match. Req: %d Act: %d\n",fileName.toChar(),Fw::DpContainer::Header::SIZE,size);
            return;
        }
        size = dataSize;
        stat = dpFile.write(dpData,size);
        if (stat != Os::File::Status::OP_OK) {
            printf("Error writing DP file data %s: status: %d\n",fileName.toChar(),stat);
            return;
        }
        if (static_cast<FwSizeType>(size) != dataSize) {
            printf("Dp file header %s write size didn't match. Req: %d Act: %d\n",fileName.toChar(),dataSize,size);
            return;
        }
        dpFile.close();
        
    }

    void DpCatalogTester::delDp(
        FwDpIdType id,
        Fw::Time& time,
        const char* dir
    ) {

        Fw::String fileName;
        fileName.format(Svc::DP_FILENAME_FORMAT,dir,id,time.getSeconds(),time.getUSeconds());
        Os::FileSystem::removeFile(fileName.toChar());
    }

    void DpCatalogTester::makeDpDir(
        const char* dir
    ) {
        Os::FileSystem::createDirectory(dir);
    }



    //! Handle a text event
    void DpCatalogTester::textLogIn(
        FwEventIdType id, //!< The event ID
        const Fw::Time& timeTag, //!< The time
        const Fw::LogSeverity severity, //!< The severity
        const Fw::TextLogString& text //!< The event string
    ) {
      TextLogEntry e = { id, timeTag, severity, text };

      printTextLogHistoryEntry(e, stdout);

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
