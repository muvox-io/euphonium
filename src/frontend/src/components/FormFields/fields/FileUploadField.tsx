import { useState } from "preact/hooks";
import { useDispatch } from "react-redux";
import { ConfigurationFileUploadField } from "../../../api/euphonium/plugins/models";
import { onFieldEvent } from "../../../redux/reducers/pluginConfigurationsReducer";
import Button from "../../ui/Button";
import { FieldProps } from "./FieldProps";
import UploadButton from "../../ui/UploadButton/UploadButton";
import getBaseUrl from "../../../api/euphonium/baseUrl";

/**
 * A field for selecting files which later get uploaded to the specified endpoint
 * @param param0
 * @returns
 */
export default function FileUploadField({
  field,
  pluginName,
}: FieldProps<ConfigurationFileUploadField>) {
  const dispatch = useDispatch();
  const onUploaded = async () => {
    try {
      await dispatch(
        onFieldEvent({
          pluginName,
          event: {
            fieldId: field.id,
            name: "upload",
            payload: "",
          },
        }) as any
      );
    } catch (e) {
      console.error(e);
    }
  };
  return (
    <div class="min-w-full flex flex-col items-start lg:flex-row lg:items-center">
      <div class="text-app-text-secondary font-light text-m mb-2 mt-2">
        {field.label}
      </div>
      <div class="grow" style={{ flexGrow: "1" }}>
        {/** fucking tailwind */}
      </div>
      <UploadButton
        buttonText={field.buttonText}
        onChange={async (e) => {
          const file = (e as any).target?.files[0];
          if (!file) return;
          const reader = new FileReader();

          reader.onload = async (event) => {
            try {
              // Once the file is read, send the raw data to the server
              const response = await fetch(
                getBaseUrl() + field.uploadEndpoint,
                {
                  method: "POST",
                  headers: {
                    "Content-Type": "application/octet-stream",
                  },
                  body: (event as any)?.target?.result,
                }
              );

              // Handle the response
              if (response.ok) {
                // file uploaded successfully
                console.log("File uploaded successfully");
                onUploaded();
              } else {
                // error occurred
                console.log("An error occurred while uploading the file");
              }
            } catch (err) {
              console.error(err);
            }
          };
          reader.readAsArrayBuffer(file);
        }}
      />
    </div>
  );
}
