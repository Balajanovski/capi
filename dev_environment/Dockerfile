FROM debian:10-slim

ARG PROXY

ENV http_proxy=${PROXY}
ENV https_proxy=${PROXY}
ENV HTTP_PROXY=${PROXY}
ENV HTTPS_PROXY=${PROXY}

WORKDIR /opt
RUN apt -y clean && apt -yqq update --fix-missing && apt -y install build-essential zlib1g-dev libncurses5-dev libgdbm-dev libnss3-dev libssl-dev libsqlite3-dev libreadline-dev libffi-dev curl libbz2-dev liblzma-dev git
RUN curl https://www.python.org/ftp/python/3.7.9/Python-3.7.9.tgz --output /opt/Python-3.7.9.tgz && tar -xf /opt/Python-3.7.9.tgz
WORKDIR /opt/Python-3.7.9
RUN ./configure --enable-optimizations && make -j 2 && make altinstall
WORKDIR /opt
RUN rm -rf Python-3.7.9

# The compilation step is needed so that pygeos and shapely could use the same version of GEOS library
RUN apt -y install libgeos-dev
RUN python3.7 -m pip install --no-cache-dir cython wheel numpy==1.20.1
RUN python3.7 -m pip install --no-cache-dir shapely==1.8a1 --no-binary shapely
RUN python3.7 -m pip install --no-cache-dir pygeos --no-binary pygeos

# Runtime for the capi library
COPY ../requirements.txt ./requirements.txt
RUN python3.7 -m pip install --no-cache-dir -r requirements.txt

# Tests
COPY ../requirements_test.txt ./requirements_test.txt
RUN python3.7 -m pip install --no-cache-dir -r requirements_test.txt
