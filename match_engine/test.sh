#!/bin/bash
docker build -t test . --build-arg my_arg="executeTests" && docker run -i --rm test /app/build/executeTests
