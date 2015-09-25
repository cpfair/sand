({
    mainConfigFile: "js/boot.js",
    baseUrl: "js/",
    paths: {
        jsx: 'ext/jsx',
        JSXTransformer: 'ext/JSXTransformer',
        text: 'ext/text'
    },
    jsx: {
        fileExtension: '.jsx'
    },
    stubModules: ['jsx', 'text', 'JSXTransformer'],
    name: "jsx!app",
    out: "js/app.min.js"
})
