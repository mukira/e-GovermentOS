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

inline std::string UFRDataSource::GetSource() { return "e-governmentos"; }

inline std::string UFRDataSource::GetMimeType(const GURL &) {
  return "text/html";
}

inline void UFRDataSource::StartDataRequest(
    const GURL &url, const content::WebContents::Getter &wc_getter,
    content::URLDataSource::GotDataCallback callback) {
  // Source content from e-GovernmentOS-agent/public/e-governmentos_first_run.html
  std::string source = R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>e-GovernmentOS</title>
    <style>
        :root {
            --primary: #fb651f;
            --primary-hover: #e05a1b;
            --bg: #FAFAFA;
            --surface: #FFFFFF;
            --text-main: #1A1A1A;
            --text-muted: #666666;
            --border: #E5E5E5;
        }

        @media (prefers-color-scheme: dark) {
            :root {
                --bg: #111111;
                --surface: #1A1A1A;
                --text-main: #FFFFFF;
                --text-muted: #A0A0A0;
                --border: #333333;
            }
        }

        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;
            margin: 0;
            padding: 0;
            background-color: var(--bg);
            color: var(--text-main);
            line-height: 1.5;
            display: flex;
            flex-direction: column;
            min-height: 100vh;
        }

        .hero {
            padding: 4rem 2rem 2rem;
            text-align: center;
        }
        
        .badge {
            background-color: var(--surface);
            border: 1px solid var(--border);
            padding: 0.5rem 1rem;
            border-radius: 2rem;
            font-size: 0.875rem;
            color: var(--text-muted);
            display: inline-block;
            margin-bottom: 2rem;
        }

        h1 {
            font-size: 3.5rem;
            font-weight: 800;
            margin: 0;
            letter-spacing: -0.02em;
        }

        .accent {
            color: var(--primary);
        }

        .subtitle {
            font-size: 1.25rem;
            color: var(--text-muted);
            max-width: 600px;
            margin: 1.5rem auto;
        }

        section {
            max-width: 1000px;
            margin: 0 auto;
            padding: 2rem;
            width: 100%;
            box-sizing: border-box;
        }

        .section-head {
            text-align: center;
            margin-bottom: 2rem;
        }

        .section-head .label {
            font-size: 1.5rem;
            font-weight: 700;
            display: inline-flex;
            align-items: center;
            gap: 0.5rem;
        }

        .cta-group {
            display: flex;
            gap: 1rem;
            justify-content: center;
            margin-top: 2rem;
        }

        .btn {
            padding: 0.75rem 1.5rem;
            border-radius: 99px;
            text-decoration: none;
            font-weight: 600;
            transition: all 0.2s;
            display: inline-flex;
            align-items: center;
            gap: 0.5rem;
        }

        .btn-primary {
            background-color: var(--primary);
            color: white;
            box-shadow: 0 4px 12px rgba(251, 101, 31, 0.25);
        }

        .btn-primary:hover {
            background-color: var(--primary-hover);
            transform: translateY(-1px);
        }

        .btn-secondary {
            background-color: var(--surface);
            border: 1px solid var(--border);
            color: var(--text-main);
        }

        .btn-secondary:hover {
            border-color: var(--text-muted);
        }

        .cards {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 1.5rem;
        }

        .card {
            background-color: var(--surface);
            border: 1px solid var(--border);
            border-radius: 1rem;
            padding: 2rem;
            transition: transform 0.2s;
        }
        
        .card.wide {
             grid-column: 1 / -1;
        }

        .card:hover {
            border-color: var(--primary);
        }

        .card h3 {
            margin: 0 0 1rem 0;
            font-size: 1.125rem;
            display: flex;
            align-items: center;
            gap: 0.75rem;
        }

        .hicon {
            width: 40px;
            height: 40px;
            border-radius: 50%;
            display: flex;
            align-items: center;
            justify-content: center;
            background-color: rgba(251, 101, 31, 0.1);
        }

        .hicon svg {
             fill: var(--primary);
             width: 24px;
             height: 24px;
        }

        .code-snippet {
            background-color: var(--bg);
            padding: 0.25rem 0.5rem;
            border-radius: 0.25rem;
            font-family: monospace;
            font-size: 0.9em;
            border: 1px solid var(--border);
        }

        .steps {
            list-style: none;
            padding: 0;
            margin: 0;
        }

        .steps li {
            margin-bottom: 0.75rem;
            padding-left: 1.5rem;
            position: relative;
            color: var(--text-muted);
        }

        .steps li::before {
            content: "•";
            color: var(--primary);
            position: absolute;
            left: 0;
            font-weight: bold;
        }
        
        .note {
            margin-top: 1rem;
            padding: 1rem;
            background-color: var(--bg);
            border: 1px dashed var(--border);
            border-radius: 0.5rem;
            font-size: 0.875rem;
            color: var(--text-muted);
        }

        .links {
            display: flex;
            justify-content: center;
            gap: 2rem;
            margin-top: 2rem;
        }
        
        .links a {
             color: var(--text-muted);
             text-decoration: none;
             display: flex;
             align-items: center;
             gap: 0.5rem;
             font-weight: 500;
        }
        
        .links a:hover {
             color: var(--primary);
        }
        
        .icon svg {
             width: 20px;
             height: 20px;
             fill: currentColor;
        }
        
        .subtle {
             font-size: 0.875rem;
             color: #999;
             margin-top: 2rem;
             padding-bottom: 2rem;
        }

    </style>
</head>
<body>

<div class="hero">
 <div class="badge">Backed by YC</div>
 <h1>The Open Source <span class="accent">Agentic Browser</span></h1>
 <p class="subtitle">e-GovernmentOS is an AI-powered browser that lets you build and run agents to automate tedious tasks. It looks like Chrome but reimagined for the AI era.</p>
</div>

<section>
 <div class="section-head"><span class="label">🚀 Getting Started</span></div>
 
 <div class="cta-group">
  <div class="btn btn-primary">Quick start guide</div>
  <a href="https://github.com/e-governmentos-ai/e-GovernmentOS" class="btn btn-secondary">
    <span class="icon" aria-hidden="true">★</span> Star us on Github
  </a>
 </div>
</section>

<section>
 <div class="cards">
  <!-- Step 1 -->
  <div class="card">
   <h3>
    <span class="hicon" aria-hidden="true">
     <svg xmlns="http://www.w3.org/2000/svg" height="24px" viewBox="0 -960 960 960" width="24px"><path d="M480-320 280-520l56-58 104 104v-326h80v326l104-104 56 58-200 200ZM240-160q-33 0-56.5-23.5T160-240v-120h80v120h480v-120h80v120q0 33-23.5 56.5T720-160H240Z"/></svg>
    </span>
    Step 1: Import your data from Chrome
   </h3>
   <ul class="steps">
    <li>Navigate to <span class="code-snippet">chrome://settings/importData</span></li>
    <li>Click "Import"</li>
    <li>Follow the prompts and choose "Always allow" when asked to import everything at once</li>
   </ul>
  </div>

  <!-- Step 2 -->
  <div class="card">
   <h3>
    <span class="hicon" aria-hidden="true">
     <svg xmlns="http://www.w3.org/2000/svg" height="24px" viewBox="0 -960 960 960" width="24px"><path d="M280-400q-33 0-56.5-23.5T200-480q0-33 23.5-56.5T280-560q33 0 56.5 23.5T360-480q0 33-23.5 56.5T280-400Zm0-160q-33 0-56.5-23.5T200-640q0-33 23.5-56.5T280-720q33 0 56.5 23.5T360-640q0 33-23.5 56.5T280-560Zm0 320q-33 0-56.5-23.5T200-320q0-33 23.5-56.5T280-400q33 0 56.5 23.5T360-320q0 33-23.5 56.5T280-240Zm200-320v-80h320v80H480Zm0 160v-80h320v80H480Zm0 160v-80h320v80H480Z"/></svg>
    </span>
    Step 2: BYOK (Bring Your Own Keys)
   </h3>
   <p style="color:var(--text-muted); font-size:0.9rem; margin-bottom:0.5rem;">You have full control over your AI models!</p>
   <p style="color:var(--text-muted); font-size:0.9rem;">Navigate to <span class="code-snippet">chrome://settings/e-governmentos</span> to configure your keys.</p>
   <div class="note">Note: You can even run everything locally using Ollama!</div>
  </div>
  
  <!-- Step 3 -->
  <div class="card wide">
   <h3>
    <span class="hicon" aria-hidden="true">
     <svg xmlns="http://www.w3.org/2000/svg" height="24px" viewBox="0 -960 960 960" width="24px"><path d="M382-240 154-468l57-57 171 171 367-367 57 57-424 424Z"/></svg>
    </span>
    Step 3: All done!
   </h3>
   <p style="color:var(--text-muted);">Your ready to use e-GovernmentOS, have fun! This page can be always accessed again at <span class="code-snippet">chrome://e-governmentos-first-run</span></p>
  </div>
 </div>
</section>

<section>
 <div class="section-head"><span class="label">🤝 Join our <span class="accent">community</span></span></div>
 <p class="links">
  <a href="https://discord.gg/YKwjt5vuKr">Discord</a>
  <a href="https://github.com/e-governmentos-ai/e-GovernmentOS">GitHub</a>
  <a href="https://x.com/browserOS_ai">X (Twitter)</a>
 </p>
 <p class="subtle" style="text-align:center;">Have questions or want to contribute? We’d love to hear from you.</p>
</section>

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
      : DefaultWebUIConfig("chrome", "e-governmentos") {}
};

class NxtscapeFirstRun : public content::WebUIController {
public:
  NxtscapeFirstRun(content::WebUI *web_ui) : content::WebUIController(web_ui) {
    content::URLDataSource::Add(Profile::FromWebUI(web_ui),
                                std::make_unique<UFRDataSource>());
  }
};

#endif // CHROME_BROWSER_UI_WEBUI_NXTSCAPE_FIRST_RUN_H_
