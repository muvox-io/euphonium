import {
  ConfigurationTable, TableColumn
} from "../../../api/euphonium/plugins/models";
import { FieldProps } from "./FieldProps";

const drawTableHeader = (columns: TableColumn[]) => {
  return (<thead>
    <tr>
      {columns.map((column) =>
        <th class="text-app-text-primary text-left pr-4">
          {column.title}
        </th>
      )}
    </tr>
  </thead>);
}

const drawTableData = (data: string[][]) => {
  return (data.map((row) =>
    <tr>
      {row.map((element) => {
        return <td class="pr-4 pr-4 text-left">{element}</td>
      })}
    </tr>
  ))
}

export default function Table({
  field,
}: FieldProps<ConfigurationTable>) {
  return (<table class="table-auto">
    {drawTableHeader(field.table_columns)}
    <tbody>
      {drawTableData(field.table_data)}
    </tbody>
  </table>)
}

// const DrawTable = (table: any) => {
//   const drawHeadRow = (row: any) =>
//     return (<thead>
//       <tr>
//         {row.map((header: any) => {
//           return (<th>{header}</th>);
//         })}
//       </tr>
//     </thead>);
//   }


//   return (<table class="table-auto">
//     {table.map((row: any) => {

//       return drawDataRow(row);
//     })}
//   </table>);
// }