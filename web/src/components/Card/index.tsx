import Icon from "../Icon";
import { route } from 'preact-router';

export default ({ title = "", subtitle = "", children = {} }) => {
  return (
    <div class='p-8 relative'>
      <div onClick={() => route('/web')} class='md:hidden shadow-m absolute text-gray-400 right-8 top-8 w-12 h-12 bg-grey-light rounded-full items-center justify-center flex'>
        <Icon name='close' />
      </div>
      <div class='text-3xl'>{title}</div>
      <div class='text-m mt-2 text-gray-400'>{subtitle}</div>
      <div class='md:shadow-lg md:bg-gray-700 md:p-6 md:rounded-2xl mt-5 mb-15'>
        {children}
      </div>
    </div>
  );
};
