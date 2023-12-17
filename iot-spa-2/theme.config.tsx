import React from "react";
import { DocsThemeConfig } from "nextra-theme-docs";

const config: DocsThemeConfig = {
  logo: <span>TransitAware: IoT Bus Seat Tracker</span>,
  project: {
    link: "https://github.com/arromaljj/iot-group-2",
  },

  docsRepositoryBase: "https://github.com/arromaljj/iot-group-2",
  darkMode: false,
  nextThemes: {
    defaultTheme: "light",
  },
  footer: {
    text: "Project by Princy, Anish, Arromal",
  },
  gitTimestamp: false,
};

export default config;
