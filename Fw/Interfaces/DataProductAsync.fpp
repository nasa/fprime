module Fw {
    @ Defines ports for asynchronous data product operations (request/recv/send)
    interface DataProductAsync {

        @ Data product request port: requests allocation of a data product container
        product request port productRequestOut

        @ Async data product recv port: receives the allocated data product container
        async product recv port productRecvIn

        @ Data product send port: sends the filled data product container
        product send port productSendOut

  }
}
