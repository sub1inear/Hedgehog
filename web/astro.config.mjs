import { defineConfig } from 'astro/config';
import mdx from '@astrojs/mdx';
import { hhgGrammar } from './src/lib/hhg-grammar.js';

export default defineConfig({
  site: 'https://hedgehog-lang.dev',
  base: '/Hedgehog',
  integrations: [
    mdx(),
  ],
  markdown: {
    shikiConfig: {
      themes: {
        light: 'github-light',
        dark: 'github-dark',
      },
      langs: [hhgGrammar],
      wrap: false,
    },
  },
});
