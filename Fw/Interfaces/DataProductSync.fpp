module Fw {
    @ Defines ports for synchronous data product operations (get/send)
    interface DataProductSync {

        @ Data product get port: allocates a data product container
        product get port productGetOut

        @ Data product send port: sends the filled data product container
        product send port productSendOut

    }
}
