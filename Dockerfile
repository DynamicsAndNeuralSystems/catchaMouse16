# Debian linux dist with python 3.10.14
FROM python:3.10.14-bookworm

# Set the working directory
WORKDIR /catchamouse_project

# Copy the current directory contents into the container
COPY . .

# Update the package index and install essential packages
RUN apt-get update && apt-get install -y libgsl-dev

# Upgrade pip and setuptools
RUN pip install --upgrade pip setuptools

# Install prerequisite packages
RUN pip install numpy pandas

# Build and install
RUN cd Python && \
    python setup_P3.py build && \
    python setup_P3.py install

# Run python when the container launches
CMD ["python"]
