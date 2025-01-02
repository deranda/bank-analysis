# Use an official Node.js image as the base image
FROM node:18-alpine

# Set the working directory inside the container
WORKDIR /app

# Copy package.json and package-lock.json to the working directory
COPY package*.json ./

# Install dependencies
RUN npm install

# Copy the rest of the application code to the container
COPY ./src ./src
COPY ./public ./public

# Expose the port the React app runs on (default: 3000)
EXPOSE 3000

# Start the React development server
CMD ["npm", "start"]
