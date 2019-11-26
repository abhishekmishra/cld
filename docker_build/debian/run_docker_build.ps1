docker run -it --rm `
    -v="d:/volumes/cld_debian_build/sw/":/sw `
    -v="d:/volumes/cld_debian_build/code/":/code `
    -v="d:/volumes/cld_debian_build/.cmake/":/root/.cmake `
    -e CLD_TEST_HOST="$CLD_TEST_HOST" `
    -e CLD_TEST_COMMAND="$CLD_TEST_COMMAND" `
    am/cld_debian_build:latest
