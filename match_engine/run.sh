#!/bin/bash
docker build -t task . --build-arg my_arg="Match_Engine" && docker run -i --rm task /app/build/Match_Engine
