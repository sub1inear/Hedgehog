/** @type {import('tailwindcss').Config} */
export default {
  content: ['./src/**/*.{astro,html,js,jsx,md,mdx,svelte,ts,tsx,vue}'],
  darkMode: 'class',
  theme: {
    extend: {
      colors: {
        quill: {
          50:  '#fff7ed',
          100: '#ffedd5',
          200: '#fed7aa',
          300: '#fdba74',
          400: '#fb923c',
          500: '#f97316',
          600: '#ea580c',
          700: '#c2410c',
          800: '#9a3412',
          900: '#7c2d12',
        },
        ink: {
          50:  '#f7f7f8',
          100: '#eeeef1',
          200: '#d9d9df',
          300: '#b8b8c2',
          400: '#8f8fa0',
          500: '#6d6d80',
          600: '#4f4f60',
          700: '#3a3a48',
          800: '#20202a',
          900: '#141419',
          950: '#0b0b10',
        },
      },
      fontFamily: {
        sans:   ['"Inter"', 'ui-sans-serif', 'system-ui', 'sans-serif'],
        display:['"Fraunces"', '"Inter"', 'serif'],
        mono:   ['"JetBrains Mono"', 'ui-monospace', 'SFMono-Regular', 'Menlo', 'monospace'],
      },
      keyframes: {
        'quill-wiggle': {
          '0%, 100%': { transform: 'rotate(-3deg)' },
          '50%':      { transform: 'rotate(3deg)' },
        },
        'peek': {
          '0%':   { transform: 'translateY(100%)' },
          '100%': { transform: 'translateY(0)' },
        },
        'glow-pulse': {
          '0%, 100%': { opacity: '0.35' },
          '50%':      { opacity: '0.7' },
        },
      },
      animation: {
        'quill-wiggle': 'quill-wiggle 3s ease-in-out infinite',
        'peek':         'peek 600ms ease-out',
        'glow-pulse':   'glow-pulse 4s ease-in-out infinite',
      },
      backgroundImage: {
        'quill-gradient': 'linear-gradient(135deg, #fb923c 0%, #f97316 50%, #ea580c 100%)',
      },
    },
  },
  plugins: [],
};
