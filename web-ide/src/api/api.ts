let apiUrl = "";

export const setApiUrl = (url: string) => {
    apiUrl = url;
}

export const getFileContent = async (filePath: string)  => {
    const response = await fetch(apiUrl + "/devtools/file/" + filePath);
    return await response.text();
}

export const updateFileContent = async (filePath: string, content: string) => {
    // const response = await fetch(apiUrl + "/file/" + filePath);
    // return response.text;
    const response = await fetch(apiUrl + "/devtools/file/" + filePath, {
        method: "POST",
        body: content,
    });
    return await response.text();
}

export const renameFile = async (oldPath: string, newPath: string) => {
    // const response = await fetch(apiUrl + "/file/" + filePath);
    // return response.text;
    const response = await fetch(apiUrl + "/devtools/rename-file", {
        method: "POST",
        body: JSON.stringify({
            currentName: oldPath,
            newName: newPath
        }),
    });
    return await response.text();
}

export const restartSystem = async () => {
    // const response = await fetch(apiUrl + "/file/" + filePath);
    // return response.text;
    const response = await fetch(apiUrl + "/system/restart", {
        method: "POST",
        body: "",
    });
    return await response.text();
}

export const getFileTree = async () => {
    const response = await fetch(apiUrl + "/devtools/file");
    const json = await response.json();
    var result: string[] = [];
    let level = { result };

    json.forEach((path: string) => {
        path.substring(1).split('/').reduce((r: any, name, i, a) => {
            if (!r[name]) {
                r[name] = { result: [] };
                r.result.push({ name, items: r[name].result, fullPath: path })
            }

            return r[name];
        }, level)
    })

    return result;
}

export const getSystemLogs = async () => {
    const response = await fetch(apiUrl + "/devtools/logs");
    return await response.text();
}