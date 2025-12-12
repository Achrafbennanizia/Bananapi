# 📚 Wallbox Portable Deploy - Documentation Index

**Package:** Wallbox Controller C++14 Portable Edition  
**Version:** 1.0  
**Created:** December 12, 2024  
**Location:** `/Users/achraf/pro/PJMT/wallbox-portable-deploy/`

---

## 📖 Documentation Files

### 1. 🚀 QUICK_START.md (4.5 KB)

**Start here if you want to deploy immediately!**

Quick deployment guide with minimal reading:

- What's in this package
- Quick deployment (2 methods)
- Testing commands
- Quick commands cheat sheet
- Pre-deployment checklist
- Success indicators

**Read this if:** You want to deploy in <5 minutes

### 2. 🔨 BUILD_METHODS.md (6 KB)

**Build system comparison and guide**

Compare Make vs CMake:

- Quick comparison table
- Make build instructions
- CMake build instructions
- Which one to use
- Example workflows
- Performance comparison
- Troubleshooting both systems
- Quick commands reference

**Read this if:** You want to understand build options

### 3. 📘 README.md (14 KB)

**Comprehensive installation and usage guide**

Everything you need to know:

- Features overview
- Requirements (hardware/software)
- Installation guide (step-by-step)
- Running the wallbox (service + manual)
- Configuration (all settings explained)
- Testing (local + remote)
- API usage (all endpoints with examples)
- UDP communication guide
- Troubleshooting (8 common issues)
- Security considerations
- Monitoring and logging
- Platform-specific notes
- Integration examples (Python, Node.js)
- Deployment checklist

**Read this if:** You want comprehensive documentation

### 3. 📦 PACKAGE_CONTENTS.md (9 KB)

**Complete package inventory and specifications**

Technical details:

- Complete file inventory (all 37 files)
- Feature completeness checklist
- Technical specifications
- C++ standard verification
- Network configuration
- Deployment methods comparison
- Testing capabilities
- Code statistics
- Security features
- Quality assurance metrics
- Performance benchmarks
- Update strategy

**Read this if:** You want technical details

### 4. ✅ CREATION_SUMMARY.md (15 KB)

**What was created and why**

Project summary:

- Mission accomplished checklist
- Complete directory structure
- All features included (detailed)
- C++14 compatibility verification
- Documentation summary
- Deployment options comparison
- Testing capabilities
- Package statistics
- Quality metrics
- Success criteria
- Next steps (when Pi is online)

**Read this if:** You want to understand what was built

---

## 🎯 Quick Navigation

### I want to...

#### Deploy the wallbox now

→ Read: **QUICK_START.md** (page 1)  
→ Run: `./scripts/deploy.sh <PI_IP> <USER>`

#### Understand how to install

→ Read: **README.md** (Installation Guide section)  
→ Run: `sudo ./scripts/install.sh`

#### Test the deployment

→ Read: **README.md** (Testing section)  
→ Run local: `./scripts/test-local.sh`  
→ Run remote: `./scripts/test-remote.sh <PI_IP>`

#### Configure the wallbox

→ Read: **README.md** (Configuration section)  
→ Edit: `config/config.json`

#### Use the API

→ Read: **README.md** (API Usage section)  
→ Test: `curl http://<PI_IP>:8080/api/status`

#### Troubleshoot issues

→ Read: **README.md** (Troubleshooting section)  
→ Check logs: `journalctl -u wallbox -n 50`

#### Understand the code

→ Read: **PACKAGE_CONTENTS.md** (Code Statistics section)  
→ Browse: `src/` and `include/` directories

#### Verify C++14 compatibility

→ Read: **CREATION_SUMMARY.md** (C++14 Compatibility section)  
→ Check: `CMakeLists.txt` (line 7: `set(CMAKE_CXX_STANDARD 14)`)

---

## 📂 Directory Structure

```
wallbox-portable-deploy/
│
├── 📚 Documentation (4 files, 42.5 KB)
│   ├── QUICK_START.md           # Quick deployment guide
│   ├── README.md                # Comprehensive documentation
│   ├── PACKAGE_CONTENTS.md      # Package inventory
│   └── CREATION_SUMMARY.md      # What was created
│
├── 🔧 Configuration (1 file)
│   └── config/
│       └── config.json          # Default configuration
│
├── 📜 Scripts (4 files)
│   └── scripts/
│       ├── install.sh           # Local installation (240 lines)
│       ├── deploy.sh            # Remote deployment (180 lines)
│       ├── test-local.sh        # Local tests (8 tests)
│       └── test-remote.sh       # Remote tests (12 tests)
│
├── 📄 Headers (13 files)
│   └── include/
│       ├── ApiController.h
│       ├── Application.h
│       ├── BananaPiGpioController.h
│       ├── ChargingStateMachine.h
│       ├── Configuration.h
│       ├── GpioFactory.h
│       ├── HttpApiServer.h
│       ├── IGpioController.h
│       ├── INetworkCommunicator.h
│       ├── SimpleWallboxController.h
│       ├── StubGpioController.h
│       ├── UdpCommunicator.h
│       └── WallboxController.h
│
├── 💻 Source (14 files)
│   └── src/
│       ├── BananaPiGpioController.cpp
│       ├── ChargingStateMachine.cpp
│       ├── HttpApiServer.cpp
│       ├── IsoStackCtrlProtocol_impl.cpp
│       ├── SimpleWallboxController.cpp
│       ├── StubGpioController.cpp
│       ├── UdpCommunicator.cpp
│       ├── WallboxController.cpp
│       ├── main.cpp
│       ├── main_v2.cpp
│       ├── main_v2_with_api.cpp
│       ├── main_v3.cpp           # ← Primary version
│       ├── main_v4.cpp
│       └── simulator.cpp
│
└── ⚙️ Build (1 file)
    └── CMakeLists.txt            # C++14 build configuration
```

**Total:** 37 files, ~6,400 lines of code, ~2 MB

---

## 🚀 Quick Start Commands

### Deploy to Pi (One Command)

```bash
cd /Users/achraf/pro/PJMT/wallbox-portable-deploy
./scripts/deploy.sh 192.168.178.34 root
```

### Test Locally (Before Deployment)

```bash
cd /Users/achraf/pro/PJMT/wallbox-portable-deploy
./scripts/test-local.sh
```

### Test Remotely (After Deployment)

```bash
./scripts/test-remote.sh 192.168.178.34
```

### Manual Install (On Pi)

```bash
# Copy to Pi first, then:
ssh root@192.168.178.34
cd wallbox-portable-deploy
sudo ./scripts/install.sh
```

### Test API

```bash
curl http://192.168.178.34:8080/api/status
```

### View Logs

```bash
ssh root@192.168.178.34 'journalctl -u wallbox -f'
```

---

## 🎯 Features at a Glance

- ✅ **C++14 Compatible** - Works on all Pi platforms
- ✅ **Dual Mode** - API + Interactive simultaneously
- ✅ **HTTP REST API** - Port 8080, JSON responses
- ✅ **UDP Communication** - ISO 15118, ports 50010/50011
- ✅ **GPIO Control** - Direct hardware control
- ✅ **Simulator** - Test without hardware
- ✅ **Systemd Service** - Auto-restart, logging
- ✅ **One-Command Deploy** - Fully automated
- ✅ **Comprehensive Tests** - 20 automated tests
- ✅ **Platform Support** - Raspberry Pi, Banana Pi, Orange Pi

---

## 📊 Package Statistics

| Item          | Count        | Size         |
| ------------- | ------------ | ------------ |
| Documentation | 4 files      | 42.5 KB      |
| Source files  | 14 files     | ~3,500 lines |
| Header files  | 13 files     | ~1,500 lines |
| Scripts       | 4 files      | ~800 lines   |
| Configuration | 1 file       | JSON         |
| Build config  | 1 file       | CMake        |
| **Total**     | **37 files** | **~2 MB**    |

---

## ✅ Deployment Checklist

When ready to deploy:

**Pre-deployment:**

- [ ] Pi has network connection
- [ ] Pi accessible via SSH: `ssh root@192.168.178.34`
- [ ] Ports 8080, 50010, 50011 available
- [ ] You have sudo/root privileges

**Deployment:**

- [ ] Run: `./scripts/deploy.sh 192.168.178.34 root`
- [ ] Wait for completion (~5-10 minutes)
- [ ] Check for errors in output

**Post-deployment:**

- [ ] Run: `./scripts/test-remote.sh 192.168.178.34`
- [ ] Test API: `curl http://192.168.178.34:8080/api/status`
- [ ] Check service: `ssh root@192.168.178.34 'systemctl status wallbox'`
- [ ] View logs: `ssh root@192.168.178.34 'journalctl -u wallbox -n 20'`
- [ ] Enable auto-start: `ssh root@192.168.178.34 'systemctl enable wallbox'`

---

## 🆘 Need Help?

### Issue: Deployment fails

**Solution:** Check SSH connection

```bash
ssh root@192.168.178.34 'echo OK'
```

### Issue: Service won't start

**Solution:** Check logs

```bash
ssh root@192.168.178.34 'journalctl -u wallbox -n 50'
```

### Issue: API not responding

**Solution:** Check firewall

```bash
ssh root@192.168.178.34 'ufw allow 8080/tcp'
curl http://192.168.178.34:8080/api/status
```

### Issue: UDP not working

**Solution:** Check ports

```bash
ssh root@192.168.178.34 'netstat -ulpn | grep 50010'
```

**More troubleshooting:** See README.md (Troubleshooting section)

---

## 📞 Quick Reference

### Access Points

- **HTTP API:** `http://<PI_IP>:8080/api/`
- **UDP Wallbox:** `<PI_IP>:50010`
- **UDP Simulator:** `<PI_IP>:50011`

### Commands

- **Start service:** `systemctl start wallbox`
- **Stop service:** `systemctl stop wallbox`
- **Restart service:** `systemctl restart wallbox`
- **View logs:** `journalctl -u wallbox -f`
- **Check status:** `systemctl status wallbox`

### Files

- **Config:** `~/wallbox-control/build/config.json`
- **Binary:** `~/wallbox-control/build/wallbox_control_v3`
- **Logs:** `journalctl -u wallbox`
- **Service:** `/etc/systemd/system/wallbox.service`

---

## 🎉 You're Ready!

Everything is prepared for deployment. When your Banana Pi (192.168.178.34) is online:

```bash
cd /Users/achraf/pro/PJMT/wallbox-portable-deploy
./scripts/deploy.sh 192.168.178.34 root
```

**That's it!** The rest is automated. 🚀

---

## 📚 Documentation Reading Order

1. **For quick deployment:** QUICK_START.md → Deploy → Done!
2. **For understanding:** README.md → PACKAGE_CONTENTS.md → CREATION_SUMMARY.md
3. **For troubleshooting:** README.md (Troubleshooting section)
4. **For technical details:** PACKAGE_CONTENTS.md
5. **For project overview:** CREATION_SUMMARY.md

---

_Last updated: December 12, 2024_  
_Package version: 1.0_  
_Status: ✅ Complete and ready for deployment_
