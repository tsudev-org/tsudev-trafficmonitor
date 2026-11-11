#!/usr/bin/env python3
"""Sinh SBOM (SPDX 2.3, JSON) cho một gói phát hành tsudev-trafficmonitor_<ver>_<arch>_Lite.zip.

Ứng dụng là C++/MFC, thư viện bên thứ ba đi kèm dạng mã nguồn nên dependency graph của GitHub
không thấy được; script liệt kê đúng các thành phần thật được biên dịch vào tsudev-trafficmonitor.exe
và mã băm SHA-256 của gói. release.yml chứng thực SBOM này bằng actions/attest-sbom:

    gh attestation verify <tệp.zip> -R tsudev-org/tsudev-trafficmonitor \
        --predicate-type https://spdx.dev/Document/v2.3

Chạy: python3 -I tools/make_sbom.py --zip dist/<tệp.zip> --version 26.11.0903 --commit <sha> > sbom.spdx.json
"""
from __future__ import annotations

import argparse
import datetime as dt
import hashlib
import json
import re
import sys
import uuid
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
REPO_URL = "https://github.com/tsudev-org/tsudev-trafficmonitor"


def tinyxml2_version() -> str:
    text = (ROOT / "tsudev-trafficmonitor/tinyxml2/tinyxml2.h").read_text(encoding="utf-8", errors="replace")
    parts = [re.search(rf"TIXML2_{p}_VERSION\s*=\s*(\d+)", text) for p in ("MAJOR", "MINOR", "PATCH")]
    if not all(parts):
        raise SystemExit("Không đọc được phiên bản tinyxml2")
    return ".".join(m.group(1) for m in parts)


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--zip", required=True)
    ap.add_argument("--version", required=True)
    ap.add_argument("--commit", required=True)
    args = ap.parse_args()

    if not re.fullmatch(r"\d+(\.\d+){0,3}", args.version) or not re.fullmatch(r"[0-9a-f]{40}", args.commit):
        print("Phiên bản hoặc commit không hợp lệ", file=sys.stderr)
        return 1
    zip_path = Path(args.zip)
    digest = hashlib.sha256(zip_path.read_bytes()).hexdigest()
    created = dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")

    app = {
        "SPDXID": "SPDXRef-Package-tsudev-trafficmonitor",
        "name": "tsudev-trafficmonitor",
        "versionInfo": args.version,
        "packageFileName": zip_path.name,
        "supplier": "Organization: tsudev (https://tsudev.com)",
        "originator": "Organization: tsudev",
        "downloadLocation": f"{REPO_URL}/releases/download/V{args.version}-vn/{zip_path.name}",
        "homepage": "https://trafficmonitor.tsudev.com",
        "sourceInfo": f"git+{REPO_URL}@{args.commit}",
        "checksums": [{"algorithm": "SHA256", "checksumValue": digest}],
        "filesAnalyzed": False,
        "licenseConcluded": "LicenseRef-Anti-996",
        "licenseDeclared": "LicenseRef-Anti-996",
        "copyrightText": "NOASSERTION",
        "attributionTexts": ["Xem LICENSE, LICENSE_CN, NOTICE.md trong gói phát hành (giấy phép Anti 996 điều 1)."],
        "primaryPackagePurpose": "APPLICATION",
        "externalRefs": [{"referenceCategory": "PACKAGE-MANAGER", "referenceType": "purl",
                          "referenceLocator": f"pkg:github/tsudev-org/tsudev-trafficmonitor@V{args.version}-vn"}],
    }
    tinyxml2 = {
        "SPDXID": "SPDXRef-Package-tinyxml2",
        "name": "tinyxml2",
        "versionInfo": tinyxml2_version(),
        "supplier": "Person: Lee Thomason",
        "downloadLocation": "https://github.com/leethomason/tinyxml2",
        "filesAnalyzed": False,
        "licenseConcluded": "Zlib",
        "licenseDeclared": "Zlib",
        "copyrightText": "NOASSERTION",
        "primaryPackagePurpose": "LIBRARY",
        "comment": "Mã nguồn đi kèm trong tsudev-trafficmonitor/tinyxml2, biên dịch tĩnh vào tsudev-trafficmonitor.exe",
        "externalRefs": [{"referenceCategory": "PACKAGE-MANAGER", "referenceType": "purl",
                          "referenceLocator": f"pkg:github/leethomason/tinyxml2@{tinyxml2_version()}"}],
    }
    mfc = {
        "SPDXID": "SPDXRef-Package-MFC-CRT",
        "name": "Microsoft Foundation Classes and Visual C++ Runtime (static, v143)",
        "versionInfo": "NOASSERTION",
        "supplier": "Organization: Microsoft Corporation",
        "downloadLocation": "NOASSERTION",
        "filesAnalyzed": False,
        "licenseConcluded": "LicenseRef-Microsoft-Visual-Studio",
        "licenseDeclared": "NOASSERTION",
        "copyrightText": "NOASSERTION",
        "primaryPackagePurpose": "LIBRARY",
        "comment": "Liên kết tĩnh khi build bằng Visual Studio 2022 (windows-2022 runner)",
    }
    doc = {
        "spdxVersion": "SPDX-2.3",
        "dataLicense": "CC0-1.0",
        "SPDXID": "SPDXRef-DOCUMENT",
        "name": f"tsudev-trafficmonitor-{args.version}-{zip_path.stem}",
        "documentNamespace": f"{REPO_URL}/spdx/{zip_path.stem}-{uuid.uuid4()}",
        "creationInfo": {"created": created, "creators": ["Tool: tools/make_sbom.py", "Organization: tsudev"]},
        "packages": [app, tinyxml2, mfc],
        "relationships": [
            {"spdxElementId": "SPDXRef-DOCUMENT", "relationshipType": "DESCRIBES", "relatedSpdxElement": app["SPDXID"]},
            {"spdxElementId": app["SPDXID"], "relationshipType": "STATIC_LINK", "relatedSpdxElement": tinyxml2["SPDXID"]},
            {"spdxElementId": app["SPDXID"], "relationshipType": "STATIC_LINK", "relatedSpdxElement": mfc["SPDXID"]},
        ],
        "hasExtractedLicensingInfos": [
            {"licenseId": "LicenseRef-Anti-996", "name": "Anti 996 License Version 1.0",
             "extractedText": "Xem tệp LICENSE trong gói phát hành."},
            {"licenseId": "LicenseRef-Microsoft-Visual-Studio", "name": "Microsoft Visual Studio license terms (redistributable)",
             "extractedText": "NOASSERTION"},
        ],
    }
    sys.stdout.reconfigure(encoding="utf-8", newline="\n")   # Windows mặc định cp1252
    json.dump(doc, sys.stdout, ensure_ascii=False, indent=2)
    print()
    return 0


if __name__ == "__main__":
    sys.exit(main())
