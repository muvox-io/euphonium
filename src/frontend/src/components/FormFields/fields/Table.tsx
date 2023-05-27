import {
  ConfigurationTable, TableColumn
} from "../../../api/euphonium/plugins/models";
import { FieldProps } from "./FieldProps";

const drawTableHeader = (columns: TableColumn[]) => {
  return (<thead>
    <tr>
      {columns.map((column) =>
        <th class={`text-${column.align} text-app-text-primary font-normal pb-2 pr-4 border-b border-app-border`}>
          {column.title}
        </th>
      )}
    </tr>
  </thead>);
}

const drawTableData = (data: string[][], columns: TableColumn[]) => {
  return (data.map((row) =>
    <tr>
      {row.map((element) => {
        const { align } = columns[row.indexOf(element)]
        return <td class={`text-${align} pt-2 pr-4 text-left`}>{element}</td>
      })}
    </tr>
  ))
}

export default function Table({
  field,
}: FieldProps<ConfigurationTable>) {
  return (<table class="table-auto lg:min-w-[400px] text-[16px] font-mono">
    {drawTableHeader(field.table_columns)}
    <tbody>
      {drawTableData(field.table_data, field.table_columns)}
    </tbody>
  </table>)
}