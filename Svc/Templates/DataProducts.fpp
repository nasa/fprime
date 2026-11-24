module Templates {

    @ Configuration for the DataProducts subtopology template
    struct DataProductsConfig {
        dpCat: ActiveComponentConfig
        dpMgr: ActiveComponentConfig
        dpWriter: ActiveComponentConfig
        dpBufferAccumulator: ActiveComponentConfig

        @ Memory identifier used by dpBufferAccumulator to allocate its queue
        bufferAccumulatorAllocatorId: FwEnumStoreType
        @ Maximum number of buffers held by dpBufferAccumulator
        bufferAccumulatorMaxNumBuffers: FwSizeType

        @ Manager identifier used by dpBufferManager for buffer checking
        bufferManagerId: U16
        @ Size of each data product buffer
        bufferStoreSize: FwSizeType
        @ Number of data product buffers
        bufferStoreCount: U16

        @ Directory where data products are written and cataloged
        dpDir: string size FileNameStringSize
        @ File storing the catalog transmit state
        dpState: string size FileNameStringSize
    } default {
        dpCat = {
            queueSize = 10
            stackSize = 64 * 1024
            $priority = 24
            cpuAffinity = Os.TASK_DEFAULT
        }
        dpMgr = {
            queueSize = 10
            stackSize = 64 * 1024
            $priority = 23
            cpuAffinity = Os.TASK_DEFAULT
        }
        dpWriter = {
            queueSize = 10
            stackSize = 64 * 1024
            $priority = 22
            cpuAffinity = Os.TASK_DEFAULT
        }
        dpBufferAccumulator = {
            queueSize = 10
            stackSize = 64 * 1024
            $priority = 21
            cpuAffinity = Os.TASK_DEFAULT
        }
        bufferAccumulatorAllocatorId = 301
        bufferAccumulatorMaxNumBuffers = 10
        bufferManagerId = 300
        bufferStoreSize = 10000
        bufferStoreCount = 10
        dpDir = "./DpCat"
        dpState = "./DpCat/DpState.dat"
    }

    @ DataProducts subtopology template
    @
    @ The expanding module must provide a C++ `Allocation::memAllocator` (Fw::MemAllocator&)
    @ in its namespace. If the deployment uses health connections, it must also provide
    @ `PingEntries::<Module>_dpCat` and `PingEntries::<Module>_dpBufferAccumulator`, where
    @ `<Module>` is the fully qualified expansion module with dots replaced by underscores.
    module template DataProducts(
        @ Base ID for the subtopology, all components are offsets from this base ID
        constant baseId: FwIdType
        @ Subtopology configuration
        constant config: DataProductsConfig
    ) {

        # ----------------------------------------------------------------------
        # Configuration constants (visible to C++ init snippets)
        # ----------------------------------------------------------------------

        module Config {
            constant dpDir                          = config.dpDir
            constant dpState                        = config.dpState
            constant bufferAccumulatorAllocatorId   = config.bufferAccumulatorAllocatorId
            constant bufferAccumulatorMaxNumBuffers = config.bufferAccumulatorMaxNumBuffers
            constant bufferManagerId                = config.bufferManagerId
            constant bufferStoreSize                = config.bufferStoreSize
            constant bufferStoreCount               = config.bufferStoreCount
        }

        # ----------------------------------------------------------------------
        # Active Components
        # ----------------------------------------------------------------------

        instance dpCat: Svc.DpCatalog base id baseId + 0x00000 \
            queue size config.dpCat.queueSize \
            stack size config.dpCat.stackSize \
            priority config.dpCat.$priority \
            cpu config.dpCat.cpuAffinity {
                phase Fpp.ToCpp.Phases.configComponents """
                Fw::FileNameString dpDir(Config::dpDir);
                Fw::FileNameString dpState(Config::dpState);
                Os::FileSystem::createDirectory(dpDir.toChar());
                Fw::ExternalArray<Fw::FileNameString> dpDirs(&dpDir, 1);
                dpCat.configure(dpDirs, dpState, 0, Allocation::memAllocator);
            """
            }

        instance dpMgr: Svc.DpManager base id baseId + 0x01000 \
            queue size config.dpMgr.queueSize \
            stack size config.dpMgr.stackSize \
            priority config.dpMgr.$priority \
            cpu config.dpMgr.cpuAffinity

        instance dpWriter: Svc.DpWriter base id baseId + 0x02000 \
            queue size config.dpWriter.queueSize \
            stack size config.dpWriter.stackSize \
            priority config.dpWriter.$priority \
            cpu config.dpWriter.cpuAffinity {
                phase Fpp.ToCpp.Phases.configComponents """
                Fw::FileNameString dpDir(Config::dpDir);
                dpWriter.configure(dpDir);
            """
            }

        instance dpBufferAccumulator: Svc.BufferAccumulator base id baseId + 0x04000 \
            queue size config.dpBufferAccumulator.queueSize \
            stack size config.dpBufferAccumulator.stackSize \
            priority config.dpBufferAccumulator.$priority \
            cpu config.dpBufferAccumulator.cpuAffinity {
                phase Fpp.ToCpp.Phases.configComponents """
                dpBufferAccumulator.allocateQueue(
                    Config::bufferAccumulatorAllocatorId,
                    Allocation::memAllocator,
                    Config::bufferAccumulatorMaxNumBuffers,
                    Svc::BufferAccumulator_OpState::DRAIN
                );
            """
                phase Fpp.ToCpp.Phases.tearDownComponents """
                dpBufferAccumulator.deallocateQueue(Allocation::memAllocator);
            """
            }

        # ----------------------------------------------------------------------
        # Passive Components
        # ----------------------------------------------------------------------

        instance dpBufferManager: Svc.BufferManager base id baseId + 0x03000 {
            phase Fpp.ToCpp.Phases.configObjects """
            Svc::BufferManager::BufferBins bins;
            """
            phase Fpp.ToCpp.Phases.configComponents """
            memset(&bins, 0, sizeof(bins));
            bins.bins[0].bufferSize = Config::bufferStoreSize;
            bins.bins[0].numBuffers = Config::bufferStoreCount;
            dpBufferManager.setup(
                Config::bufferManagerId,
                0,
                Allocation::memAllocator,
                bins
            );
            """
            phase Fpp.ToCpp.Phases.tearDownComponents """
            dpCat.shutdown();
            dpBufferManager.cleanup();
            """
        }

        topology Subtopology {
            #Active Components
            instance dpCat
            instance dpMgr
            instance dpWriter
            instance dpBufferAccumulator

            #Passive Components
            instance dpBufferManager

            connections DataProducts {
                # DpMgr, BufferAccumulator, and DpWriter connections. Have explicit port indexes for demo
                dpMgr.bufferGetOut[0]                   -> dpBufferManager.bufferGetCallee
                dpMgr.productSendOut[0]                 -> dpBufferAccumulator.bufferSendInFill
                dpBufferAccumulator.bufferSendOutDrain  -> dpWriter.bufferSendIn
                dpWriter.deallocBufferSendOut           -> dpBufferAccumulator.bufferSendInReturn
                dpBufferAccumulator.bufferSendOutReturn -> dpBufferManager.bufferSendIn

                dpWriter.dpWrittenOut -> dpCat.addToCat
            }

            # ----------------------------------------------------------------------
            # Topology ports
            # ----------------------------------------------------------------------

            @ Input port array for responding to data product get requests from client components
            port productGetIn = dpMgr.productGetIn

            @ Input port array for receiving data product buffer requests from client components
            port productRequestIn = dpMgr.productRequestIn

            @ Input port array for receiving filled data product buffers from client components
            port productSendIn = dpMgr.productSendIn

            @ Output port array for sending requested data product buffers to client components
            port productResponseOut = dpMgr.productResponseOut

            @ Output port for sending file downlink requests to a file downlink component
            port dpCatFileOut = dpCat.fileOut

            @ Input port for receiving file downlink completion notifications
            port dpCatFileDone = dpCat.fileDone

            @ Input port for scheduling dpBufferManager telemetry output
            port dpBufferManagerSchedIn = dpBufferManager.schedIn

            @ Input port for scheduling dpWriter telemetry output
            port dpWriterSchedIn = dpWriter.schedIn

            @ Output port for processing data products
            port dpWriterProcOut = dpWriter.procBufferSendOut

            @ Input port for scheduling dpMgr telemetry output
            port dpMgrSchedIn = dpMgr.schedIn

        } # end topology

    } # end DataProducts template

}
