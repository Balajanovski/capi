FROM python:3.8.9-slim

ARG PROXY

ENV http_proxy=${PROXY}
ENV https_proxy=${PROXY}
ENV HTTP_PROXY=${PROXY}
ENV HTTPS_PROXY=${PROXY}

WORKDIR /opt
RUN apt -y clean && \
    apt -yqq update --fix-missing && \
    apt -y install gcc-8 \
                   g++-8 \
                   libstdc++6 \
                   build-essential \
                   git \
                   libpcre3 \
                   libpcre3-dev \
                   automake \
                   bison \
                   flex \
                   zlib1g-dev \
                   libncurses5-dev \
                   libgdbm-dev \
                   libnss3-dev \
                   libssl-dev \
                   libsqlite3-dev \
                   libreadline-dev \
                   libffi-dev \
                   curl \
                   libbz2-dev \
                   liblzma-dev

RUN python -m pip install --upgrade pip

# The compilation step is needed so that pygeos and shapely could use the same version of GEOS library
RUN apt -y install libgeos-dev
RUN python -m pip install --no-cache-dir cython wheel numpy==1.20.1
RUN python -m pip install --no-cache-dir shapely==1.8a1 --no-binary shapely
RUN python -m pip install --no-cache-dir pygeos --no-binary pygeos

# Runtime for the capi library
COPY ../requirements.txt ./requirements.txt
RUN python -m pip install --no-cache-dir -r requirements.txt

# Tests
COPY ../requirements_test.txt ./requirements_test.txt
RUN python -m pip install --no-cache-dir -r requirements_test.txt
