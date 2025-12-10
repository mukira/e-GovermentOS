// Copyright 2025 E-Nation OS. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_WEBUI_NXTSCAPE_FIRST_RUN_H_
#define CHROME_BROWSER_UI_WEBUI_NXTSCAPE_FIRST_RUN_H_

#include "base/memory/ref_counted_memory.h"
#include "chrome/browser/profiles/profile.h"
#include "content/public/browser/url_data_source.h"
#include "content/public/browser/web_ui_controller.h"
#include "content/public/browser/webui_config.h"

// Define the First Run WebUI
class UFRDataSource : public content::URLDataSource {
public:
  UFRDataSource() = default;
  ~UFRDataSource() override = default;

  std::string GetSource() override;
  std::string GetMimeType(const GURL &) override;
  void
  StartDataRequest(const GURL &url,
                   const content::WebContents::Getter &wc_getter,
                   content::URLDataSource::GotDataCallback callback) override;
};

inline std::string UFRDataSource::GetSource() { return "browseros-first-run"; }

inline std::string UFRDataSource::GetMimeType(const GURL &) {
  return "text/html";
}

inline void UFRDataSource::StartDataRequest(
    const GURL &url, const content::WebContents::Getter &wc_getter,
    content::URLDataSource::GotDataCallback callback) {
  std::string source = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Welcome to BrowserOS</title>
    <style>
        body { margin: 0; padding: 0; display: flex; align-items: center; justify-content: center; height: 100vh; font-family: system-ui, sans-serif; background: #fff; }
        .container { text-align: center; max-width: 600px; }
        h1 { font-size: 3rem; margin-bottom: 1rem; color: #fb651f; }
        p { font-size: 1.2rem; color: #555; }
        .logo { width: 100px; height: 100px; margin-bottom: 2rem; }
    </style>
</head>
<body>
    <div class="container">
        <h1>BrowserOS</h1>
        <p>The Open-Source Agentic Browser</p>
    </div>
</body>
</html>
  )";
  std::move(callback).Run(
      base::MakeRefCounted<base::RefCountedString>(std::move(source)));
}

class NxtscapeFirstRun;
class NxtscapeFirstRunUIConfig
    : public content::DefaultWebUIConfig<NxtscapeFirstRun> {
public:
  NxtscapeFirstRunUIConfig()
      : DefaultWebUIConfig("chrome", "browseros-first-run") {}
};

class NxtscapeFirstRun : public content::WebUIController {
public:
  NxtscapeFirstRun(content::WebUI *web_ui) : content::WebUIController(web_ui) {
    content::URLDataSource::Add(Profile::FromWebUI(web_ui),
                                std::make_unique<UFRDataSource>());
  }
};

#endif // CHROME_BROWSER_UI_WEBUI_NXTSCAPE_FIRST_RUN_H_
