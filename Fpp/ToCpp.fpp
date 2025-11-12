module Fpp {

  module ToCpp {

    enum Phases : U8 {
      configConstants
      configObjects
      instances
      initComponents
      configComponents
      regCommands
      readParameters
      loadParameters
      startTasks
      stopTasks
      freeThreads
      tearDownComponents
    }

  }

}
