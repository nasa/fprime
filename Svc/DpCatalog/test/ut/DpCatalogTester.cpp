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
#include "Fw/Types/FileNameString.hpp"
#include "config/DpCfg.hpp"
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

        Fw::FileNameString dirs[2];
        dirs[0] = "dir0";
        dirs[1] = "dir1";
        this->component.configure(dirs,FW_NUM_ARRAY_ELEMENTS(dirs),100,alloc);
        this->component.shutdown();
    }

    //! Read one DP test
    void DpCatalogTester::readDps(
            Fw::FileNameString *dpDirs,
            FwSizeType numDirs,
            const DpSet *dpSet,
            FwSizeType numDps
        ) {
 
        // make a directory for the files
        for (FwSizeType dir = 0; dir < numDirs; dir++) {
            this->makeDpDir(dpDirs[dir].toChar());
        }

        // clean up last DP
        for (FwSizeType dp = 0; dp < numDps; dp++) {
            this->delDp(
                dpSet[dp].id,
                dpSet[dp].time,
                dpSet[dp].dir
            );

            this->genDP(
                dpSet[dp].id,
                dpSet[dp].prio,
                dpSet[dp].time,
                dpSet[dp].dataSize,
                dpSet[dp].state,
                false,
                dpSet[dp].dir
                );
        }

        Fw::MallocAllocator alloc;

        this->component.configure(dpDirs,numDirs,100,alloc);

        this->sendCmd_BUILD_CATALOG(0,10);
        this->component.doDispatch();
        this->sendCmd_START_XMIT_CATALOG(0,0,Fw::Wait::NO_WAIT);
        this->component.doDispatch();

        // dispatch messages
        for (FwSizeType msg = 0; msg < numDps; msg++) {
            // dispatch file done port call
            this->component.doDispatch();
        }

        this->component.shutdown();

    }


    void DpCatalogTester::genDP(
        FwDpIdType id,
        FwDpPriorityType prio,
        const Fw::Time& time,
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
        fileName.format(DP_FILENAME_FORMAT,dir,id,time.getSeconds(),time.getUSeconds());
        COMMENT(fileName.toChar());
        Os::File dpFile;
        Os::File::Status stat = dpFile.open(fileName.toChar(),Os::File::Mode::OPEN_CREATE);
        if (stat != Os::File::Status::OP_OK) {
            printf("Error opening file %s: status: %d\n",fileName.toChar(),stat);
            return;
        }
        FwSignedSizeType size = Fw::DpContainer::Header::SIZE;
        stat = dpFile.write(hdrData,size);
        if (stat != Os::File::Status::OP_OK) {
            printf("Error writing DP file header %s: status: %d\n",fileName.toChar(),stat);
            return;
        }
        if (static_cast<FwSizeType>(size) != Fw::DpContainer::Header::SIZE) {
            printf("Dp file header %s write size didn't match. Req: %" PRI_FwSignedSizeType "Act: %" PRI_FwSignedSizeType "\n",fileName.toChar(),Fw::DpContainer::Header::SIZE,size);
            return;
        }
        size = dataSize;
        stat = dpFile.write(dpData,size);
        if (stat != Os::File::Status::OP_OK) {
            printf("Error writing DP file data %s: status: %" PRI_FwNativeIntType "\n",fileName.toChar(),stat);
            return;
        }
        if (static_cast<FwSizeType>(size) != dataSize) {
            printf("Dp file header %s write size didn't match. Req: %" PRI_FwSignedSizeType " Act: %" PRI_FwSignedSizeType "\n",fileName.toChar(),dataSize,size);
            return;
        }
        dpFile.close();
        
    }

    void DpCatalogTester::delDp(
        FwDpIdType id,
        const Fw::Time& time,
        const char* dir
    ) {

        Fw::String fileName;
        fileName.format(DP_FILENAME_FORMAT,dir,id,time.getSeconds(),time.getUSeconds());
        Os::FileSystem::removeFile(fileName.toChar());
    }

    void DpCatalogTester::makeDpDir(
        const char* dir
    ) {
        Os::FileSystem::Status stat = Os::FileSystem::createDirectory(dir);
        if (stat != Os::FileSystem::Status::OP_OK) {
            printf("Couldn't create directory %s\n",dir);
        }
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
            const Fw::StringBase& sourceFileName,
            const Fw::StringBase& destFileName,
            U32 offset,
            U32 length
        )
    {
        this->invoke_to_fileDone(0,Svc::SendFileResponse());

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
