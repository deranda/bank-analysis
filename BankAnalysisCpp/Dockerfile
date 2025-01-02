# Use an official Qt base image
FROM ubuntu:24.04

# Set environment variables to avoid interaction during installation
ENV DEBIAN_FRONTEND=noninteractive

# Install necessary dependencies
RUN apt-get update && apt-get install -y \
    && rm -rf /var/lib/apt/lists/*
	
# Set working directory inside the container
WORKDIR /app

# Copy your Qt project files into the container
COPY run/* /app/

RUN ls -l /app/
RUN chmod +x /app/backendCpp
ENV LD_LIBRARY_PATH=/app/libs:
ENV XDG_SESSION_TYPE=offscreen
ENV QT_QPA_PLATFORM=offscreen

# Expose the port where the backend listens (replace with your actual port)
EXPOSE 8080

# Run the Qt application
CMD ["./backendCpp"]
