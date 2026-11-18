// Version switcher for the documentation published on GitHub Pages.
//
// The published site has one directory per version (master and all tags) and a
// switcher.json at the site root that lists all of them (in the format of the
// pydata-sphinx-theme version switcher). This script adds a dropdown to the
// sidebar of the sphinx_rtd_theme that allows to switch to the same page in a
// different version. If switcher.json cannot be found (e.g. for local builds)
// nothing is added.
(function () {
  "use strict";

  const SWITCHER_FILE = "switcher.json";
  const VERSION_RE = /^[\w.-]+$/;

  // The theme styles the switcher rather faint, make it a bit more visible
  const SWITCHER_STYLE = `
    .wy-side-nav-search > div.switch-menus,
    .wy-side-nav-search > div.switch-menus > div.version-switch select {
      color: hsla(0, 0%, 100%, 0.6);
    }
    .wy-side-nav-search > div.switch-menus > div.version-switch select:hover,
    .wy-side-nav-search > div.switch-menus > div.version-switch select:focus {
      color: hsla(0, 0%, 100%, 0.9);
    }`;

  function setup() {
    const contentRoot = document.documentElement.dataset.content_root;
    if (contentRoot === undefined || window.location.protocol === "file:") {
      return;
    }

    // Root of the documentation of this version and of the whole site
    const versionRoot = new URL(contentRoot, window.location.href);
    const siteRoot = new URL("../", versionRoot);
    const currentVersion = versionRoot.pathname.split("/").filter(Boolean).pop();
    const pagePath = window.location.pathname.slice(versionRoot.pathname.length);

    fetch(new URL(SWITCHER_FILE, siteRoot), { cache: "no-cache" })
      .then((response) => (response.ok ? response.json() : Promise.reject(response.status)))
      .then((entries) => {
        const versions = entries.filter(
          (entry) => typeof entry.version === "string" && VERSION_RE.test(entry.version),
        );
        if (versions.length > 0) {
          addSwitcher(versions, currentVersion, (version) =>
            switchTo(new URL(`${version}/`, siteRoot), pagePath),
          );
        }
      })
      .catch(() => {
        // No (valid) switcher.json, e.g. local or PR build, nothing to do
      });
  }

  function addSwitcher(versions, currentVersion, onSwitch) {
    const select = document.createElement("select");
    select.setAttribute("aria-label", "Documentation version");

    for (const entry of versions) {
      const option = new Option(entry.name || entry.version, entry.version);
      option.selected = entry.version === currentVersion;
      select.add(option);
    }
    // Make sure the current version is shown even if it is not in the list
    if (currentVersion && !versions.some((entry) => entry.version === currentVersion)) {
      select.add(new Option(currentVersion, currentVersion, true, true), 0);
    }

    select.addEventListener("change", () => onSwitch(select.value));

    // Use the markup that sphinx_rtd_theme already has styling for
    const versionSwitch = document.createElement("div");
    versionSwitch.className = "version-switch";
    versionSwitch.append(select);
    const menus = document.createElement("div");
    menus.className = "switch-menus";
    menus.append(versionSwitch);

    const sideNav = document.querySelector(".wy-side-nav-search");
    if (!sideNav) {
      return;
    }
    const style = document.createElement("style");
    style.textContent = SWITCHER_STYLE;
    document.head.append(style);

    const search = sideNav.querySelector("div[role='search']");
    sideNav.insertBefore(menus, search);
  }

  // Go to the same page in the target version if it exists, otherwise to the
  // start page of the target version
  async function switchTo(targetRoot, pagePath) {
    const targetPage = new URL(pagePath, targetRoot);
    try {
      const response = await fetch(targetPage, { method: "HEAD" });
      if (response.ok) {
        window.location.href = targetPage.href + window.location.hash;
        return;
      }
    } catch (err) {
      // Fall through to the start page
    }
    window.location.href = targetRoot.href;
  }

  if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", setup);
  } else {
    setup();
  }
})();
