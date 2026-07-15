event StoreAllocationFailed(moduleCount: U16, err: AllocError) \
    severity warning high \
    format "Failed to allocate store with {} modules: {}"

event StoreAllocationSucceeded(moduleCount: U16) \
    severity activity low \
    format "Successfully allocated store with {} modules"
