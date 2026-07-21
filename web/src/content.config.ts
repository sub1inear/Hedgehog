import { defineCollection, z } from 'astro:content';
import { glob } from 'astro/loaders';

const docs = defineCollection({
  loader: glob({
    pattern: '**/*.md',
    base: './src/content/docs',
    generateId: ({ data }) => (data as { slug: string }).slug,
  }),
  schema: z.object({
    title: z.string(),
    description: z.string().optional(),
    order: z.number(),
    section: z.string().default('Guide'),
    slug: z.string(),
  }),
});

export const collections = { docs };
