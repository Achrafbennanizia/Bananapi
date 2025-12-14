# SSH Key Authentication Setup

## ✅ Configuration Complete

### SSH Key Details

**Key Type:** ED25519 (modern, secure, fast)  
**Private Key:** `~/.ssh/bananapi_key`  
**Public Key:** `~/.ssh/bananapi_key.pub`  
**Fingerprint:** SHA256:739xpeSSs8uOMEEVs6z4CGdZNXFzlCsy0SuNg5JZOfk

### SSH Config Aliases

Two convenient aliases have been added to `~/.ssh/config`:

```ssh-config
Host bananapi
    HostName 192.168.178.34
    User root
    IdentityFile ~/.ssh/bananapi_key
    ServerAliveInterval 60
    ServerAliveCountMax 3

Host bananapim5
    HostName bananapim5.fritz.box
    User root
    IdentityFile ~/.ssh/bananapi_key
    ServerAliveInterval 60
    ServerAliveCountMax 3
```

## Usage

### Simple SSH Access

No password required anymore! Just use:

```bash
# Connect by IP
ssh bananapi

# Or by hostname
ssh bananapim5

# Or explicit
ssh -i ~/.ssh/bananapi_key root@192.168.178.34
```

### Quick Commands

```bash
# Check wallbox status
ssh bananapi 'pgrep -f wallbox_control_v3'

# View logs
ssh bananapi 'tail -f /tmp/wallbox.out'

# Restart wallbox
ssh bananapi 'killall wallbox_control_v3; cd ~/wallbox-src/build && nohup ./wallbox_control_v3 > /tmp/wallbox.out 2>&1 &'

# Check system info
ssh bananapi 'hostname && uptime && free -h'
```

### File Transfer

```bash
# Using updated TRANSFER.sh (now uses private key)
cd /Users/achraf/pro/PJMT/wallbox-deployment
./TRANSFER.sh bananapi

# Direct rsync with key
rsync -avz -e "ssh -i ~/.ssh/bananapi_key" ./file root@192.168.178.34:~/

# Or using SSH config alias
rsync -avz ./file bananapi:~/
```

### SCP Examples

```bash
# Copy to Banana Pi
scp config.json bananapi:~/wallbox-src/build/

# Copy from Banana Pi
scp bananapi:/tmp/wallbox.out ./logs/

# Copy entire directory
scp -r ./configs/ bananapi:~/
```

## Security Benefits

✅ **No Password Required** - Faster, more convenient  
✅ **More Secure** - Key-based authentication stronger than passwords  
✅ **No Password Exposure** - Passwords can't be intercepted  
✅ **Automated Scripts** - Can run scripts without manual password entry  
✅ **SSH Agent Support** - Can use `ssh-agent` for added security  
✅ **Key Rotation** - Easy to rotate keys if needed

## Key Management

### Backup Private Key

```bash
# Backup to secure location
cp ~/.ssh/bananapi_key ~/secure-backup/bananapi_key.backup

# Set strict permissions
chmod 600 ~/secure-backup/bananapi_key.backup
```

### Remove Key Access (if needed)

```bash
# On Mac (remove local key)
rm ~/.ssh/bananapi_key ~/.ssh/bananapi_key.pub

# On Banana Pi (remove authorized key)
ssh bananapi 'sed -i "/achraf@mac-to-bananapi/d" ~/.ssh/authorized_keys'
```

### Add Key to Another Machine

```bash
# Copy public key to another computer
ssh-copy-id -i ~/.ssh/bananapi_key.pub user@another-machine
```

## Updated Scripts

### TRANSFER.sh

The deployment transfer script now automatically uses the private key:

```bash
# Old way (prompted for password)
./TRANSFER.sh 192.168.178.34 root

# New way (uses private key, no password)
./TRANSFER.sh bananapi

# Also works with IP (auto-detects key)
./TRANSFER.sh 192.168.178.34 root
```

## Connection Settings

**ServerAliveInterval:** 60 seconds  
**ServerAliveCountMax:** 3 attempts

These settings keep the SSH connection alive and automatically reconnect if the network hiccups.

## Troubleshooting

### Permission Errors

If you get permission errors, check key permissions:

```bash
chmod 600 ~/.ssh/bananapi_key
chmod 644 ~/.ssh/bananapi_key.pub
chmod 700 ~/.ssh
```

### Connection Refused

Check if SSH service is running on Banana Pi:

```bash
ssh bananapi 'systemctl status sshd'
```

### Key Not Working

Test with verbose output:

```bash
ssh -v bananapi
```

### Still Asking for Password

Check if the public key is properly installed:

```bash
ssh bananapi 'cat ~/.ssh/authorized_keys | grep "achraf@mac-to-bananapi"'
```

## Quick Reference Commands

```bash
# Connect
ssh bananapi

# Transfer files
./TRANSFER.sh bananapi

# Check wallbox
ssh bananapi 'pgrep wallbox'

# View logs
ssh bananapi 'tail -f /tmp/wallbox.out'

# Monitor system
ssh bananapi 'htop'

# Restart wallbox
ssh bananapi 'cd ~/wallbox-src/build && ./wallbox_control_v3'
```

---

**Setup Date:** December 10, 2024  
**Status:** ✅ Active and Working
