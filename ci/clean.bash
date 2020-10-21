####
# clean.bash:
#
# Cleans FPRIME_DIR if set, otherwise cleans ".". Note: only will work if the lock-out key
# "CI_CLEAN_REPO" is set to "NO_REALLY_I_WANT_TO_NUKE_ALL_YOUR_BASE". This is to allow the CI system
# to clean the repo over and over while protecting  innocent developers from nuking updates....again.
####
if [[ "${CI_CLEAN_REPO}" == "NO_REALLY_I_WANT_TO_NUKE_ALL_YOUR_BASE" ]]
then
    echo -e "${BLUE}Cleaning Repository at ${FPRIME_DIR:-.}${NOCOLOR}"
    git clean -xdffe "ci-*" "${FPRIME_DIR:-.}" 1>/dev/null 2>/dev/null || fail_and_stop "Failed to clean git repository before testing"
fi
