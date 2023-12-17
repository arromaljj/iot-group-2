const withNextra = require("nextra")({
  theme: "nextra-theme-docs",
  themeConfig: "./theme.config.tsx",
});

const withVideos = require("next-videos");

module.exports = withVideos(withNextra());
