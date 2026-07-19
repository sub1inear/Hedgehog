/** TextMate grammar for the Hedgehog language, shared by astro.config.mjs and the playground. */
export const hhgGrammar = {
  name: 'hedgehog',
  aliases: ['hhg'],
  scopeName: 'source.hhg',
  fileTypes: ['hhg'],
  patterns: [
    { include: '#comment-line' },
    { include: '#comment-block' },
    { include: '#string' },
    { include: '#keyword-control' },
    { include: '#keyword-decl' },
    { include: '#keyword-logical' },
    { include: '#constant-lang' },
    { include: '#type-builtin' },
    { include: '#number' },
    { include: '#function-call' },
    { include: '#operator' },
  ],
  repository: {
    'comment-line': {
      name: 'comment.line.double-slash.hhg',
      match: '//.*$',
    },
    'comment-block': {
      name: 'comment.block.hhg',
      begin: '/\\*',
      end: '\\*/',
    },
    'string': {
      name: 'string.quoted.double.hhg',
      begin: 'f?"',
      end: '"',
      patterns: [
        { name: 'meta.interpolation.hhg', match: '\\{[^}]*\\}' },
        { name: 'constant.character.escape.hhg', match: '\\\\.' },
      ],
    },
    'keyword-control': {
      name: 'keyword.control.hhg',
      match: '\\b(if|else|while|for|in|match|break|continue|return)\\b',
    },
    'keyword-decl': {
      name: 'keyword.other.declaration.hhg',
      match: '\\b(let|mut|const|fn|class|interface|enum|type|import|from|as|pub|unsafe|static)\\b',
    },
    'keyword-logical': {
      name: 'keyword.operator.word.hhg',
      match: '\\b(and|or|not)\\b',
    },
    'constant-lang': {
      name: 'constant.language.hhg',
      match: '\\b(true|false|null|self)\\b',
    },
    'type-builtin': {
      name: 'support.type.primitive.hhg',
      match: '\\b(i8|u8|i16|u16|i32|u32|i64|u64|f32|f64|bool|char|usize|isize|void|str|list|dict|unique|shared|weak)\\b',
    },
    'number': {
      patterns: [
        { name: 'constant.numeric.hex.hhg',     match: '\\b0[xX][0-9a-fA-F_]+\\b' },
        { name: 'constant.numeric.binary.hhg',  match: '\\b0[bB][01_]+\\b' },
        { name: 'constant.numeric.octal.hhg',   match: '\\b0[oO][0-7_]+\\b' },
        { name: 'constant.numeric.float.hhg',   match: '\\b\\d[\\d_]*\\.\\d[\\d_]*([eE][+-]?\\d+)?\\b' },
        { name: 'constant.numeric.integer.hhg', match: '\\b\\d[\\d_]*\\b' },
      ],
    },
    'function-call': {
      name: 'entity.name.function.hhg',
      match: '\\b([a-zA-Z_][a-zA-Z0-9_]*)(?=\\s*\\()',
    },
    'operator': {
      name: 'keyword.operator.hhg',
      match: '(\\.\\.=?|<<=?|>>=?|->|[=!<>]=|[+\\-*/%&|^]=|[+\\-*/%<>&|^~!?]|=)',
    },
  },
};
