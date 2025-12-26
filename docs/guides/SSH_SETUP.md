# SSH Key Authentication for Deployment

This guide explains how to set up SSH key-based authentication for secure, password-less deployment to embedded systems.

## Overview

SSH key authentication provides:

- **Security**: No password transmission over network
- **Convenience**: No password prompts during deployment
- **Automation**: Enable automated deployment scripts
- **Audit Trail**: Track which keys have access

## Prerequisites

- SSH client installed on development machine
- SSH server running on target device
- Network connectivity between machines

## Generate SSH Key Pair

### Recommended: ED25519 (Modern & Secure)

```bash
ssh-keygen -t ed25519 -C "wallbox-deployment-key"
```

### Alternative: RSA (Legacy Compatibility)

```bash
ssh-keygen -t rsa -b 4096 -C "wallbox-deployment-key"
```

### Key Generation Options

- **`-t`**: Key type (ed25519 recommended, rsa for compatibility)
- **`-b`**: Bit length (4096 for RSA)
- **`-C`**: Comment to identify the key
- **`-f`**: Output file path (optional)

**Example**:

```bash
ssh-keygen -t ed25519 -f ~/.ssh/wallbox_deploy -C "wallbox-deployment"
```

## Copy Public Key to Target

### Method 1: Using ssh-copy-id (Easiest)

```bash
ssh-copy-id -i ~/.ssh/wallbox_deploy.pub user@target-device
```

### Method 2: Manual Copy

```bash
# On development machine
cat ~/.ssh/wallbox_deploy.pub

# On target device
mkdir -p ~/.ssh
chmod 700 ~/.ssh
echo "PUBLIC_KEY_CONTENT" >> ~/.ssh/authorized_keys
chmod 600 ~/.ssh/authorized_keys
```

### Method 3: Using SCP

```bash
scp ~/.ssh/wallbox_deploy.pub user@target-device:~/key.pub
ssh user@target-device 'cat ~/key.pub >> ~/.ssh/authorized_keys && rm ~/key.pub'
```

## Configure SSH Client

Edit `~/.ssh/config` for convenient connection:

```sshconfig
Host wallbox-pi
    HostName <TARGET_IP>
    User <TARGET_USER>
    IdentityFile ~/.ssh/wallbox_deploy
    ServerAliveInterval 60
    ServerAliveCountMax 3

Host wallbox-*
    IdentityFile ~/.ssh/wallbox_deploy
    StrictHostKeyChecking ask
    UserKnownHostsFile ~/.ssh/known_hosts
```

### Configuration Options

- **HostName**: IP address or hostname of target device
- **User**: Username on target device
- **IdentityFile**: Path to private key
- **ServerAliveInterval**: Keep connection alive (seconds)
- **ServerAliveCountMax**: Max failed keep-alive attempts
- **StrictHostKeyChecking**: Security check for new hosts

## Test Connection

```bash
# Using alias
ssh wallbox-pi

# Using full command
ssh -i ~/.ssh/wallbox_deploy user@target-ip
```

## Use in Deployment Scripts

### Update Deploy Script

```bash
#!/bin/bash
TARGET_IP="$1"
TARGET_USER="${2:-root}"
SSH_KEY="$HOME/.ssh/wallbox_deploy"

# Deploy using key
rsync -avz -e "ssh -i $SSH_KEY" ./build/ $TARGET_USER@$TARGET_IP:~/wallbox/
ssh -i $SSH_KEY $TARGET_USER@$TARGET_IP 'cd ~/wallbox && ./install.sh'
```

### Deployment Script Usage

```bash
./scripts/deploy/deploy.sh <target-ip> [username]
```

## Security Best Practices

### 1. Protect Private Key

```bash
# Set strict permissions
chmod 600 ~/.ssh/wallbox_deploy

# Verify permissions
ls -la ~/.ssh/wallbox_deploy
# Should show: -rw------- (600)
```

### 2. Use Passphrase (Optional)

Add extra security layer:

```bash
ssh-keygen -p -f ~/.ssh/wallbox_deploy
```

### 3. Restrict Key Usage

On target device, edit `~/.ssh/authorized_keys`:

```bash
# Restrict to specific commands
command="/usr/local/bin/deploy-only.sh" ssh-ed25519 AAAA...

# Restrict source IP
from="192.168.1.0/24" ssh-ed25519 AAAA...

# No port forwarding
no-port-forwarding,no-X11-forwarding,no-agent-forwarding ssh-ed25519 AAAA...
```

### 4. Regular Key Rotation

```bash
# Generate new key
ssh-keygen -t ed25519 -f ~/.ssh/wallbox_deploy_new

# Copy to all targets
ssh-copy-id -i ~/.ssh/wallbox_deploy_new.pub user@target

# Remove old key from targets
ssh user@target 'sed -i "/old-key-comment/d" ~/.ssh/authorized_keys'

# Update local config
mv ~/.ssh/wallbox_deploy_new ~/.ssh/wallbox_deploy
```

## Key Management

### Backup Keys

```bash
# Backup to secure location
cp ~/.ssh/wallbox_deploy* ~/secure-backup/
chmod 600 ~/secure-backup/wallbox_deploy*
```

### List Active Keys

```bash
# On development machine
ls -la ~/.ssh/wallbox_deploy*

# On target device
cat ~/.ssh/authorized_keys
```

### Revoke Key Access

```bash
# On target device
ssh user@target 'sed -i "/wallbox-deployment-key/d" ~/.ssh/authorized_keys'

# Or remove entire authorized_keys
ssh user@target 'rm ~/.ssh/authorized_keys'
```

### Multiple Keys

```bash
# Different keys for different purposes
~/.ssh/wallbox_deploy_prod    # Production
~/.ssh/wallbox_deploy_dev     # Development
~/.ssh/wallbox_deploy_test    # Testing

# Configure in ~/.ssh/config
Host wallbox-prod
    IdentityFile ~/.ssh/wallbox_deploy_prod

Host wallbox-dev
    IdentityFile ~/.ssh/wallbox_deploy_dev
```

## Troubleshooting

### Permission Denied

```bash
# Check key permissions
ls -la ~/.ssh/wallbox_deploy  # Should be 600
chmod 600 ~/.ssh/wallbox_deploy

# Verify public key on target
ssh user@target 'cat ~/.ssh/authorized_keys'
```

### Connection Timeout

```bash
# Test basic connectivity
ping target-ip

# Verify SSH service
ssh -v user@target-ip  # Verbose output

# Check firewall
# On target: sudo ufw status
# Allow SSH: sudo ufw allow 22/tcp
```

### Wrong Key

```bash
# Specify key explicitly
ssh -i ~/.ssh/wallbox_deploy -vvv user@target

# Check SSH agent
ssh-add -l  # List loaded keys
ssh-add ~/.ssh/wallbox_deploy  # Add key to agent
```

### Host Key Changed

```bash
# Remove old host key
ssh-keygen -R target-ip

# Or edit known_hosts
nano ~/.ssh/known_hosts
```

## Automation Tips

### SSH Agent

```bash
# Start agent
eval "$(ssh-agent -s)"

# Add key
ssh-add ~/.ssh/wallbox_deploy

# Verify
ssh-add -l

# Add to shell profile for persistence
echo 'eval "$(ssh-agent -s)"' >> ~/.bashrc
echo 'ssh-add ~/.ssh/wallbox_deploy' >> ~/.bashrc
```

### Batch Deployment

```bash
# Deploy to multiple targets
for host in wallbox-{1..5}; do
    echo "Deploying to $host..."
    ./scripts/deploy/deploy.sh $host
done
```

### CI/CD Integration

```bash
# Store private key as secret in CI/CD
# Example: GitHub Actions
- name: Deploy
  env:
    SSH_PRIVATE_KEY: ${{ secrets.WALLBOX_DEPLOY_KEY }}
  run: |
    echo "$SSH_PRIVATE_KEY" > ~/.ssh/deploy_key
    chmod 600 ~/.ssh/deploy_key
    ./scripts/deploy/deploy.sh target-ip
```

## Benefits

✅ **No Password Required** - Seamless authentication  
✅ **Enhanced Security** - Stronger than password authentication  
✅ **Audit Trail** - Track access by key  
✅ **Automation Friendly** - Non-interactive deployment  
✅ **Multi-Device** - Same key for multiple targets  
✅ **Revocable** - Easy to revoke compromised keys

## See Also

- [Deployment Guide](guides/INSTALLATION.md#deployment-to-embedded-systems)
- [Security Best Practices](../README.md#security)
- [SSH Manual](https://man.openbsd.org/ssh)
- [SSH Key Types](https://security.stackexchange.com/questions/5096)

---

_For production environments, consider using SSH certificate authentication for centralized key management._
