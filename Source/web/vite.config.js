
import { defineConfig } from "vite";
import zipPack from "vite-plugin-zip-pack";

export default defineConfig({
  plugins: [zipPack()],
});

