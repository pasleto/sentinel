FROM node:fermium

LABEL author="pasleto <tomas.pasler@gmail.com>" app="Sentinel" description="Sentinel"

# Create app directory
RUN mkdir -p /usr/src/sentinel
WORKDIR /usr/src/sentinel

# Set the node env (we only need production dependencies in the deployed image)
ENV NODE_ENV=production

# Install dependencies (we deliberately just copy packages.json so we can use the cache if no package.json changes are made)
COPY package.json /usr/src/sentinel
RUN npm install --silent

# Copy the sources
COPY . /usr/src/sentinel

# Start the app!
# CMD [ "npm", "start" ] #?