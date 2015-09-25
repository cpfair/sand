require.config({
                baseUrl: "static/js/",
                paths: {
                    jsx: 'ext/jsx',
                    JSXTransformer: 'ext/JSXTransformer',
                    text: 'ext/text',
                    react: 'ext/React',
                    classnames: 'ext/classNames',
                    reqwest: 'ext/reqwest.min',
                    underscore: 'ext/underscore-min'
                },
                jsx: {
                    fileExtension: '.jsx'
                },
                name: "jsx!app",
            });

